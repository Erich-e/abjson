#ifndef ABJSON_JSTATE
#define ABJSON_JSTATE
#include "jDefines.h"
#include <map>
#include <vector>

namespace abJSON
{
  enum class JState {
    start,
    array,
    mapKey,
    mapValue,
    pushArray,
    pushMap,
    popping,
    end,
  };

  // We will define the map at initialization to capture the
  // instance of the state manager
  // typedef std::map<JOP, JState> jOpMap;
  using jOpMap = std::map<JOP, JState>;

  class jStateManager
  {
  private:
    JState                    myCurrent;
    std::map<JState, jOpMap>  myOracle;
    std::vector<JState>       myStateStack;
    bool                      myError;

  public:
    jStateManager();
    ~jStateManager();

    JState getState() const;
    void nextState(JOP operation);

    bool fail() const;
    void clear();
    void reset();
    operator bool() const;
  };
}
#endif