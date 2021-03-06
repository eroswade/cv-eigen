/* detect_corners.cpp
 *
 * Author: Fabian Meyer
 * Created On: 18 Jun 2019
 */

#include <iostream>
#include <knn/brute_force.h>
#include <cve/feature/fast_detector.h>
#include <cve/feature/brief_extractor.h>
#include <cve/feature/orb_extractor.h>
#include <cve/draw/match_drawer.h>
#include <cve/draw/colors.h>
#include <cve/imageio/imageio.h>

using namespace cve;

typedef Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> Matrix;

int main(int argc, const char **argv)
{
    if(argc != 3)
    {
        std::cerr << "usage: find_matches <filename> <filename>" << std::endl;
        return -1;
    }

    std::cout << "Load \"" << argv[1] << "\" and \"" << argv[2] << "\"" << std::endl;
    Image8 imgA;
    Image8 imgB;
    Image8 grayA;
    Image8 grayB;
    Imagef oimg;
    Matrix keypointsA;
    Matrix keypointsB;
    Matrixi indices;
    Matrixi32 distances;
    Matrixi32 descriptorsA;
    Matrixi32 descriptorsB;
    std::string ext = cve::extension(argv[1]);
    knn::BruteForce<int32_t, knn::HammingDistance<int32_t>> bf;
    MatchDrawer<float> matchDrawer;

    cve::imload(argv[1], imgA);
    cve::imload(argv[2], imgB);

    cve::image::rgb2gray(imgA, grayA);
    cve::image::rgb2gray(imgB, grayB);

    std::cout << "Apply FAST detector" << std::endl;
    FASTDetector<float> fast;
    fast(grayA, keypointsA);
    fast(grayB, keypointsB);

    std::cout << "kpA:" << keypointsA.cols() << " kpB:" << keypointsB.cols() << std::endl;

    std::cout << "Compute BRIEF descriptors" << std::endl;
    BRIEFExtractor<float> brief;
    brief(grayA, keypointsA, descriptorsA);
    brief(grayB, keypointsB, descriptorsB);

    bf.setData(descriptorsB);
    bf.setMaxDistance(40);
    bf.query(descriptorsA, 1, indices, distances);

    matchDrawer.draw(imgA, imgB,
        keypointsA,
        keypointsB,
        indices,
        oimg);

    cve::imsave("brief_matches." + ext, oimg);

    std::cout << "Compute ORB descriptors" << std::endl;
    ORBExtractor<float> orb;
    orb(grayA, keypointsA, descriptorsA);
    orb(grayB, keypointsB, descriptorsB);

    bf.setData(descriptorsB);
    bf.setMaxDistance(30);
    bf.query(descriptorsA, 1, indices, distances);

    matchDrawer.draw(imgA, imgB,
        keypointsA,
        keypointsB,
        indices,
        oimg);

    cve::imsave("orb_matches." + ext, oimg);

    return 0;
}
