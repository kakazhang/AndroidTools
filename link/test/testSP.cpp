#include <stdio.h>
#include <unistd.h>
#include <utils/RefBase.h>

using namespace android;

class TestSP : public RefBase {
public:
    TestSP() {
        printf("hello cons\n");
    }

    virtual ~TestSP() {
        printf("hello desc\n");
    }
};

int main() {
    {
        sp<TestSP> tsp = new TestSP();
        sleep(1);
        //tsp = NULL;
    }

    return 0;
}
