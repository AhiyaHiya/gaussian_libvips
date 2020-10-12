
#include <boost/program_options.hpp>
#include <filesystem>
#include <iostream>
#include <vips/vips.h>

namespace fs = std::filesystem;
namespace po = boost::program_options;

using namespace std;

////////////////////////////////////////////////////////////////////////////////////////
class VipsExit
{
public:
  VipsExit() { ; }
  ~VipsExit()
  {
    printf("vips_shutdown\n");
    vips_shutdown();
  }
};

////////////////////////////////////////////////////////////////////////////////////////
/// This is needed for calling unref when a VipsImage goes out of scope.
class OnExit
{
public:
  OnExit(VipsImage* image_)
    : image(image_)
  {
    if (image)
      printf("[%d] Tracking image: %p\n", __LINE__, image);
  }
  ~OnExit()
  {
    if (image)
      printf("[%d] About to unref image: %p\n", __LINE__, image);

    if (image)
      g_object_unref(image);
  }

  VipsImage* image;
};

////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
  auto appStart = std::chrono::high_resolution_clock::now();

  // Expects parameter passed in is the image to apply the gaussian blur to
  // Expects second parameter to be the output path to where you want to save the blurred image.
  // Setup program options
  auto desc = po::options_description("Options");
  // clang-format off
  desc.add_options()
  ("help,h", "Show help message")
  ("source,s", po::value< string >(), "The full file path to the source image to blur")
  ("destination,d", po::value< string >(), "The full file path to where to save the blurred image")
  ("blur,b", po::value< int >(), "The blur intensity to use");
  // clang-format on

  // Parse the passed in options
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  // Now act on what we have
  if (vm.count("help"))
  {
    cout << desc << "\n";
    return 1;
  }

  string sourcePath;
  string destPath;
  int blur = -1;

  if (vm.count("source"))
  {
    sourcePath = vm["source"].as<string>();
  }
  else
  {
    printf("Source path parameter missing\n");
    return -1;
  }
  if (vm.count("destination"))
  {
    destPath = vm["destination"].as<string>();
  }
  else
  {
    printf("Destination path parameter missing\n");
    return -1;
  }
  if (vm.count("blur"))
  {
    blur = vm["blur"].as<int>();
  }
  else
  {
    printf("Blur parameter missing\n");
    return -1;
  }

  printf("Init the VIPS library\n");
  string nameOfApp = "gaussian_libvips";
  if (VIPS_INIT(nameOfApp.c_str()))
  {
    printf("Error detected; underlying image processing library did not start up properly");
    return false;
  }

  VipsExit vipsCleanup;

  // Load up the image
  auto sourceImage = vips_image_new_from_file(sourcePath.c_str(), nullptr);
  if (!sourceImage)
  {
    printf("[%d] #### Error detected with vips_image_new_from_file operation", __LINE__);
    return -1;
  }
  OnExit a(sourceImage);

  // Perform blur filter (note blur value is sigma of Gaussian blur)
  // Note, if you wanted to convert box filter kernel size to sigma, use...
  // auto sigma = sqrt (((kernelSize * kernelSize) - 1)/12);
  //
  if (vips_gaussblur(sourceImage, &sourceImage, blur, nullptr) != 0)
  {
    printf("Error detected with vips_gaussblur operation\n");
    return -1;
  }

  if (vips_image_write_to_file(sourceImage, destPath.c_str(), nullptr, nullptr) != 0)
  {
    printf("[%d] #### Error detected with vips_image_write_to_file operation", __LINE__);
    return -1;
  }

  auto appStop = std::chrono::high_resolution_clock::now();
  auto appDuration = std::chrono::duration_cast<std::chrono::microseconds>(appStop - appStart).count();
  printf("App time: %lu\n", appDuration);

  printf("All done\n");
  return 0;
}
