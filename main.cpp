#include "reader.hpp"

/**
 * @brief Example of how to read XML files.
 * @return 0 if the application terminated normally.
 */
int main()
{
    /* ===========================================================================================
     *                        XML file in swapped format, like Orzan et al.
     * =========================================================================================== */
    const char* path1 = "../scenes/curve_only/poivron_orzan.xml";
    scene scene1(path1);
    std::cout << "----------------------------------------------------------------" << std::endl;
    std::cout << "Successfully read XML file: " << path1 << std::endl;
    std::cout << "Image dimensions: " << scene1.width << " x " << scene1.height << std::endl;
    std::cout << "Number of diffusion curves: " << scene1.diffusion_curves.size() << std::endl;

    /* ===========================================================================================
     *                         General XML file - Diffusion Curves
     * =========================================================================================== */
    const char* path2 = "../scenes/curve_only/test_curve.xml";
    scene scene2(path2);
    std::cout << "----------------------------------------------------------------" << std::endl;
    std::cout << "Successfully read XML file: " << path2 << std::endl;
    std::cout << "Image dimensions: " << scene2.width << " x " << scene2.height << std::endl;
    std::cout << "Number of diffusion curves: " << scene2.diffusion_curves.size() << std::endl;

    /* ===========================================================================================
     *                         General XML file - Gradient Meshes
     * =========================================================================================== */
    const char* path3 = "../scenes/mesh_backgrounds/sea.xml";
    // const char* path3 = "../scenes/mesh_backgrounds/orange_sky.xml";
    // const char* path3 = "../scenes/mesh_backgrounds/blue_sky.xml";
    // const char* path3 = "../scenes/mesh_backgrounds/pink_sky.xml";
    // const char* path3 = "../scenes/mesh_backgrounds/snow_sky.xml";
    // const char* path3 = "../scenes/mesh_backgrounds/sunset.xml";
    // const char* path3 = "../scenes/mesh_backgrounds/mondrian.xml";
    scene scene3(path3);
    std::cout << "----------------------------------------------------------------" << std::endl;
    std::cout << "Successfully read XML file: " << path3 << std::endl;
    std::cout << "Image dimensions: " << scene3.width << " x " << scene3.height << std::endl;
    std::cout << "Number of gradient meshes: " << scene3.gradient_meshes.size() << std::endl;

    /* ===========================================================================================
     *                         XML file - Multiple primitives
     * =========================================================================================== */
    const char* path4 = "../scenes/unified/bubble.xml";
    // const char* path4 = "../scenes/unified/crane.xml";
    // const char* path4 = "../scenes/unified/crane_ribbons.xml";
    // const char* path4 = "../scenes/unified/drape.xml";
    // const char* path4 = "../scenes/unified/ladybug.xml";
    // const char* path4 = "../scenes/unified/pepper.xml";
    // const char* path4 = "../scenes/unified/portal.xml";
    // const char* path4 = "../scenes/unified/sunset.xml";
    // const char* path4 = "../scenes/unified/sunset_illustration.xml";
    scene scene4(path4);
    std::cout << "----------------------------------------------------------------" << std::endl;
    std::cout << "Successfully read XML file: " << path4 << std::endl;
    std::cout << "Image dimensions: " << scene4.width << " x " << scene4.height << std::endl;
    std::cout << "Number of diffusion curves: " << scene4.diffusion_curves.size() << std::endl;
    std::cout << "Number of Poisson curves: " << scene4.poisson_curves.size() << std::endl;
    std::cout << "Number of gradient meshes: " << scene4.gradient_meshes.size() << std::endl;

    return 0;
}
