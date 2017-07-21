#pragma once

#include <vector>
#include <pcl/io/io.h>
#include <limits>

#include "cluster.h"

namespace hc
{
    struct triplet
    {
        size_t pointIndexA;
        size_t pointIndexB;
        size_t pointIndexC;
        Eigen::Vector3f center;
        Eigen::Vector3f direction;
        float error;
    };

    typedef std::vector<size_t> cluster;

    struct cluster_group
    {
        std::vector<cluster> clusters;
        float bestClusterDistance;
    };

    struct cluster_history
    {
        std::vector<triplet> triplets;
        std::vector<cluster_group> history;
    };

    typedef std::function<float(triplet const &lhs, triplet const &rhs, pcl::PointCloud<pcl::PointXYZI>::ConstPtr cloud)> TripletMetric;
    typedef std::function<float(cluster const &lhs, cluster const &rhs, Eigen::MatrixXf const &d, pcl::PointCloud<pcl::PointXYZI>::ConstPtr cloud)> ClusterMetric;

    inline float defaultTripletMetric(triplet const &lhs, triplet const &rhs, pcl::PointCloud<pcl::PointXYZI>::ConstPtr cloud)
    {
        float const perpendicularDistanceA = (rhs.center - (lhs.center + lhs.direction.dot(rhs.center - lhs.center) * lhs.direction)).squaredNorm();
        float const perpendicularDistanceB = (lhs.center - (rhs.center + rhs.direction.dot(lhs.center - rhs.center) * rhs.direction)).squaredNorm();

        float const angle = 1.0f - std::abs(lhs.direction.dot(rhs.direction));

        std::cout << perpendicularDistanceA << ", " << perpendicularDistanceB << ", " << std::max(perpendicularDistanceA, perpendicularDistanceB) << ',' << std::pow(2.0f, 1.0f + 12.0f * angle) << std::endl;

        // squared distances!
        return std::max(perpendicularDistanceA, perpendicularDistanceB) + std::pow(2.0f, 1.0f + 12.0f * angle);
    }

    inline float singleLinkClusterMetric(cluster const &lhs, cluster const &rhs, Eigen::MatrixXf const &d, pcl::PointCloud<pcl::PointXYZI>::ConstPtr cloud)
    {
        float result = std::numeric_limits<float>::infinity();

        for (size_t const &a : lhs)
        {
            for (size_t const &b : rhs)
            {
                float distance = d(a, b);

                if (distance < result)
                    result = distance;
            }
        }

        return result;
    }

    inline float completeLinkClusterMetric(cluster const &lhs, cluster const &rhs, Eigen::MatrixXf const &d, pcl::PointCloud<pcl::PointXYZI>::ConstPtr cloud)
    {
        float result = 0.0f;

        for (size_t const &a : lhs)
        {
            for (size_t const &b : rhs)
            {
                float distance = d(a, b);

                if (distance > result)
                    result = distance;
            }
        }

        return result;
    }

    std::vector<triplet> generateTriplets(pcl::PointCloud<pcl::PointXYZI>::ConstPtr cloud, size_t nnKandidates = 12, size_t nBest = 2, float maxError = 0.015f);
    cluster_history calculateHc(pcl::PointCloud<pcl::PointXYZI>::ConstPtr cloud, std::vector<triplet> const &triplets, ClusterMetric clusterMetric = singleLinkClusterMetric, TripletMetric tripletMetric = defaultTripletMetric);
    cluster_group getBestClusterGroup(cluster_history const &history, float bestClusterDistanceDelta = 19.0f);
    cluster_group cleanupClusterGroup(cluster_group const &clusterGroup, size_t minTriplets = 7);
    Cluster toCluster(std::vector<hc::triplet> const &triplets, hc::cluster_group const &clusterGroup, size_t pointIndexCount);
}
