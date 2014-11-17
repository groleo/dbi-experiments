#include <stdio.h>
namespace gpu {
namespace gles2 {
class GLES2Implementation {
public:
	GLES2Implementation() {}
	void Viewport() {printf("mutatee: %s\n",__PRETTY_FUNCTION__);}
};
}
}

int main(int argc, const char** argv) {
	gpu::gles2::GLES2Implementation o;
	o.Viewport();
	return 0;
}
