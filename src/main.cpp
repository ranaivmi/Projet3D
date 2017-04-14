#include "../include/Viewer.hpp"
#include "../include/log.hpp"

#include <sstream>

// forward declararion of scene building functions, in separate files
void initialize_scene(Viewer& viewer, unsigned int scene_to_load = 71);


int main(int argc, char* argv[])
{
    Viewer viewer(1280, 720);   // window size in pixels

    unsigned int scene_to_load = 75;

    for (unsigned int i = 1; i < argc; ++i) {
        std::istringstream tokenizer(std::string(argv[i]));
        tokenizer >> scene_to_load;
        if (tokenizer.fail()) {
            LOG(fatal, "unknown argument " << argv[i] << ". Please launch without argument or the number of the practical you want to load");
            exit(EXIT_FAILURE);
        }
    }
    initialize_scene(viewer, scene_to_load);

    while (viewer.isRunning()) {
        viewer.handleEvent();
        viewer.animate();
        viewer.draw();
        viewer.display();
    }

    return EXIT_SUCCESS;
}
