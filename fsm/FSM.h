#pragma once

#include <vector>
#include <map>
#include <cassert>

template <typename T>
class FSM
{
private:
	typedef void ( T::*TransitionFunc ) ( void );
	typedef void ( T::*TickFunc ) ( double );

	struct Transition
	{
		int from;
		int to;
		TransitionFunc func;

		Transition( int from, int to, TransitionFunc func )
		{
			this->from = from;
			this->to = to;
			this->func = func;
		}
	};

	int mLastState;
	int mCurrState;
	double mTimeInState;
	int mPendingTransition;

	T* mInstance;
	std::vector <Transition> mTransitions;
	std::map< int, TickFunc> mStates;

	int transitionIndex(int from, int to);

public:
	static const int INVALID_STATE = -1;
	static const int NO_TRANSITION = -1;

	int lastState() { return mLastState; }
	int state() 	{ return mCurrState; }
	double timeInState() { return mTimeInState; }

	FSM();
	void init( T* instance );
	
	void addState( int state, TickFunc func );
	void addTransition( int from, int to, TransitionFunc func = NULL );
	void transitionTo( int to );

	void tick( double dt );
};

#include "FSM.tpp"