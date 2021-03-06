#pragma once
#include <vector>
#include "ODefines.hpp"
#include "EventSystem.hpp"



namespace EventSystem
{
  // Handles connecting and disconnecting of events based on their IDs.
  // This service is provided for objects only, and is not something that 
  // is provided for global functions that are part of the event system.
  class ObjectEventManager
  {
  public:
    // Constructor and Destructor
    ObjectEventManager(EventSystem &es);
    virtual ~ObjectEventManager();

    // Member Functions
    template <typename Caller, typename EventType>
    void Connect(Caller *c, void(Caller::*func)(EventType *));
		template <typename Caller, typename EventToBind>
		void ConnectVague(Caller *c, void(Caller::*func)(Event *));
    //void Disconnect(O_ID toDisconnect);

  private:
    // Variables
    std::vector<O_ID> connections_;
    EventSystem &eventSystem_;

	ObjectEventManager& operator=(const ObjectEventManager& other) = delete;
  };
}

#include "ObjectEventManager.tpp"
