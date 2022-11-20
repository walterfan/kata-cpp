#include <string>
#include <iostream>
#include <memory>
#include <variant>
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <optional>
#include <map>

//http://www.vishalchovatiya.com/state-design-pattern-in-modern-cpp/
using namespace std;

enum TaskAction {
    HOLD,
    PLAN,
    START,
    STOP
};

enum TaskStateName {
    NONE,
    PLANED,
    STARTED,
    STOPPED
};



struct TaskState {
    virtual shared_ptr<TaskState> onAction(TaskAction action) = 0;
    virtual TaskStateName getTaskStateName() const = 0;
    virtual ~TaskState() {}
};


struct TaskStateFactory: public std::enable_shared_from_this<TaskStateFactory>  {
    shared_ptr<TaskState> createTaskState(TaskStateName stateName);
    template< typename T> shared_ptr<TaskState> makeTaskState(TaskStateName stateName);
    map<TaskStateName, shared_ptr<TaskState>> mStates;
};


struct AbstractTaskState: public TaskState {
    void registerFactory(shared_ptr<TaskStateFactory> stateFactory);
    shared_ptr<TaskStateFactory> mStateFactory;
};


struct TaskStateNone: public AbstractTaskState {
    virtual shared_ptr<TaskState> onAction(TaskAction action);
    virtual TaskStateName getTaskStateName() const { return TaskStateName::NONE; }
    virtual ~TaskStateNone() = default;
};

struct TaskStatePlaned: public AbstractTaskState {
    virtual shared_ptr<TaskState> onAction(TaskAction action);
    virtual TaskStateName getTaskStateName() const { return TaskStateName::PLANED; }
    virtual ~TaskStatePlaned() = default;
    time_t mStartTime;
};

struct TaskStateStarted: public AbstractTaskState {
    virtual shared_ptr<TaskState> onAction(TaskAction action);
    virtual TaskStateName getTaskStateName() const { return TaskStateName::STARTED; }
    virtual ~TaskStateStarted() = default;
};

struct TaskStateStopped: public AbstractTaskState {
    virtual shared_ptr<TaskState> onAction(TaskAction action);
    virtual TaskStateName getTaskStateName() const { return TaskStateName::STOPPED; }
    virtual ~TaskStateStopped() = default;

};


struct TaskController {
    TaskController();
    void dispatch(TaskAction action);

    shared_ptr<TaskStateFactory> mStateFactory = make_shared<TaskStateFactory>();
    shared_ptr<TaskState> mState;

};




inline ostream &operator<<(ostream &os, const TaskState &state) {
    switch (state.getTaskStateName()) {
        case TaskStateName::NONE: os << "None"; break;
        case TaskStateName::PLANED: os << "Planed"; break;
        case TaskStateName::STARTED: os << "Started"; break;
        case TaskStateName::STOPPED: os << "Stopped"; break;
    }

    return os;
}