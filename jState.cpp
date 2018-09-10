#include "jState.h"

using namespace abJSON;

jStateManager::jStateManager()
  : myCurrent(JState::start)
  , myError(false)
{
  // Define the default OpMap to manage state
  std::map<JState, jOpMap> oracle;

  // transitions from the starting state
  jOpMap startMap;

  startMap[JOP::null] = JState::end;
  startMap[JOP::boolean] = JState::end;
  startMap[JOP::number] = JState::end;
  startMap[JOP::string] = JState::end;
  startMap[JOP::beginArray] = JState::pushArray;
  startMap[JOP::beginMap] = JState::pushMap;

  // transitions from withing an array
  jOpMap arrayMap;

  arrayMap[JOP::null] = JState::array;
  arrayMap[JOP::boolean] = JState::array;
  arrayMap[JOP::number] = JState::array;
  arrayMap[JOP::string] = JState::array;
  arrayMap[JOP::beginArray] = JState::pushArray;
  arrayMap[JOP::endArray] = JState::popping;
  arrayMap[JOP::beginMap] = JState::pushMap;

  // transitions for a key
  jOpMap mapKeyMap;

  mapKeyMap[JOP::key] = JState::mapValue;
  mapKeyMap[JOP::endMap] = JState::popping;

  jOpMap mapValueMap;

  mapValueMap[JOP::null] = JState::mapKey;
  mapValueMap[JOP::boolean] = JState::mapKey;
  mapValueMap[JOP::number] = JState::mapKey;
  mapValueMap[JOP::string] = JState::mapKey;
  mapValueMap[JOP::beginArray] = JState::pushArray;
  mapValueMap[JOP::beginMap] = JState::pushMap;

  // populate the oracle with the transition data
  oracle[JState::start] = startMap;
  oracle[JState::array] = arrayMap;
  oracle[JState::mapKey] = mapKeyMap;
  oracle[JState::mapValue] = mapValueMap;

  myOracle = oracle;
}

jStateManager::~jStateManager()
{
}

JState jStateManager::getState() const
{
  return myCurrent;
}

void jStateManager::nextState(JOP operation)
{
  if (myOracle[myCurrent].count(operation))
  {
    JState nextState = myOracle[myCurrent][operation];
    switch (nextState)
    {
      case JState::popping:
        // Once we pop, we want to tell the state below that we have finished the object
        // hack this by sending a null op
        myCurrent = myStateStack.back();
        myStateStack.pop_back();
        myCurrent = myOracle[myCurrent][JOP::null];
        break;
      case JState::pushArray:
        myStateStack.push_back(myCurrent);
        myCurrent = JState::array;
        break;
      case JState::pushMap:
        myStateStack.push_back(myCurrent);
        myCurrent = JState::mapKey;
        break;
      default:
        myCurrent = nextState;
    }
  }
  else
  {
    myError = true;
  }
}

bool jStateManager::fail() const
{
  return myError;
}

void jStateManager::clear()
{
  myError = false;
}

void jStateManager::reset()
{
  clear();
  myCurrent = JState::start;
  myStateStack.clear();
}

jStateManager::operator bool() const
{
  return !fail();
}
