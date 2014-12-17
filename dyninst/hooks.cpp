#include <stdio.h>
namespace gpu {
namespace gles2 {
class GLES2Implementation {
public:
	GLES2Implementation() {}
	void Viewport(int a, int b, int c, int d) {printf("HOOK mutatee: %s\n",__PRETTY_FUNCTION__);}
};
}
}

extern "C" {
void preHook() {
	gpu::gles2::GLES2Implementation o;
	o.Viewport(0,0,1,1);
	printf("%s called\n", __FUNCTION__);
}
}

