#include <stdio.h>
namespace gpu {
namespace gles2 {
class GLES2Implementation {
public:
	GLES2Implementation() {}
	void Viewport(int a, int b, int c, int d) {printf("mutatee: %s\n",__PRETTY_FUNCTION__);}
};
}
}

extern "C" {
int doubler(int num)
{
	printf("mutatee: double this : %d\n",num);
	return 0;
}
}

int main(int argc, const char** argv) {
	gpu::gles2::GLES2Implementation o;
	o.Viewport(0,0,100,100);
	doubler(3);
	return 0;
}
