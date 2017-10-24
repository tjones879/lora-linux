#include <bandit/bandit.h>

using namespace snowhouse;
using namespace bandit;

// Tell bandit there are tests here.
go_bandit([]() {
  // We're describing how a fuzzbox works.
  describe("fuzzbox", []() {


    it("starts in clean mode", [&]() {
      AssertThat(1, Equals(1));
    });
  });
});

int main(int argc, char* argv[]) {
  // Run the tests.
  return bandit::run(argc, argv);
}
