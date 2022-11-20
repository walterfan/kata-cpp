#include "state_pattern.h"

template<typename T>
shared_ptr<TaskState> TaskStateFactory::makeTaskState(TaskStateName stateName) {
    auto it = mStates.find(stateName);
    if (it != mStates.end()) {
        return it->second;
    }
    auto state = make_shared<T>();
    mStates[stateName] = state;
    state->registerFactory(shared_from_this());
    return state;
}

shared_ptr<TaskState> TaskStateFactory::createTaskState(TaskStateName stateName) {
    switch (stateName) {
        case TaskStateName::NONE:     return makeTaskState<TaskStateNone>(stateName);
        case TaskStateName::STARTED:  return makeTaskState<TaskStateStarted>(stateName);
        case TaskStateName::PLANED:   return makeTaskState<TaskStatePlaned>(stateName);
        case TaskStateName::STOPPED:  return makeTaskState<TaskStateStopped>(stateName);
    }
}

void AbstractTaskState::registerFactory(shared_ptr<TaskStateFactory> stateFactory) {
    mStateFactory = stateFactory;
}

shared_ptr<TaskState> TaskStateNone::onAction(TaskAction action) {
    switch (action) {
        case TaskAction::PLAN: return mStateFactory->createTaskState(TaskStateName::PLANED);
        case TaskAction::START: return mStateFactory->createTaskState(TaskStateName::STARTED);
        case TaskAction::HOLD: return nullptr;
        case TaskAction::STOP: return nullptr;
    }
}

shared_ptr<TaskState> TaskStatePlaned::onAction(TaskAction action) {
    switch (action) {
        case TaskAction::STOP: return mStateFactory->createTaskState(TaskStateName::NONE);
        case TaskAction::START: return mStateFactory->createTaskState(TaskStateName::STARTED);
        case TaskAction::HOLD: return nullptr;
        case TaskAction::PLAN: return nullptr;
    }

}

shared_ptr<TaskState> TaskStateStarted::onAction(TaskAction action) {
    switch (action) {
        case TaskAction::STOP: return mStateFactory->createTaskState(TaskStateName::STOPPED);
        case TaskAction::HOLD: return nullptr;
        case TaskAction::PLAN: return nullptr;
        case TaskAction::START: return nullptr;
    }
}

shared_ptr<TaskState> TaskStateStopped::onAction(TaskAction action) {
    switch (action) {
        case TaskAction::PLAN: return mStateFactory->createTaskState(TaskStateName::PLANED);
        case TaskAction::START: return mStateFactory->createTaskState(TaskStateName::STARTED);
        case TaskAction::HOLD: return nullptr;
        case TaskAction::STOP: return nullptr;
    }
    return nullptr;
}

TaskController::TaskController() {
    mState = mStateFactory->createTaskState(TaskStateName::NONE);
}

void TaskController::dispatch(TaskAction action) {
    auto new_state = mState->onAction(action);

    if (new_state) {
        cout << "TaskState changed to " << *new_state << endl;
        mState = move(new_state);
    } else {
        cout << "TaskState kept to " << *mState << endl;
    }

}

int main(int argc, char* argv[])
{
    cout << "Dispatch Task Action ..." << endl;
    shared_ptr<TaskController> controller = make_shared<TaskController>();
    controller->dispatch(TaskAction::HOLD);
    controller->dispatch(TaskAction::PLAN);
    controller->dispatch(TaskAction::START);
    controller->dispatch(TaskAction::STOP);

    return 0;
}
