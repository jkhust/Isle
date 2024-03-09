template <typename T>
FSM<T>::FSM()
{
}

// ----------------------------------------------------------------------------------
template <typename T>
void FSM<T>::init( T* instance )
{
	mLastState = INVALID_STATE;
	mCurrState = INVALID_STATE;
	mPendingTransition = NO_TRANSITION;
	mTimeInState = 0.0f;
	mInstance = instance;
}

// ----------------------------------------------------------------------------------
template <typename T>
void FSM<T>::addState( int state, TickFunc func )
{
    assert( mStates[state] == NULL );

	mStates[state] = func;
	if ( mCurrState == INVALID_STATE )
	{
		mLastState = state;
		mCurrState = state;
		mPendingTransition = NO_TRANSITION;
		mTimeInState = 0.0f;
	}
}

// ----------------------------------------------------------------------------------
template <typename T>
void FSM<T>::addTransition( int from, int to, TransitionFunc func )
{
	assert( transitionIndex( from, to ) == NO_TRANSITION );
	mTransitions.push_back( Transition( from, to, func ) );
}

// ----------------------------------------------------------------------------------
template <typename T>
int FSM<T>::transitionIndex( int from, int to )
{
	for ( int i = 0; i < mTransitions.size(); ++i )
	{
		if ( mTransitions[i].from == from && mTransitions[i].to == to )
		{
			return i;
		}
	}
	return NO_TRANSITION;
}

// ----------------------------------------------------------------------------------
template <typename T>
void FSM<T>::transitionTo( int to )
{
	mPendingTransition = transitionIndex( mCurrState, to );
	assert( mPendingTransition != NO_TRANSITION );
}

// ----------------------------------------------------------------------------------
template <typename T>
void FSM<T>::tick( double dt )
{
	assert( mInstance );

	if ( mPendingTransition != NO_TRANSITION )
	{
		TransitionFunc transitionFunc = mTransitions[mPendingTransition].func;
		if ( transitionFunc != NULL )
		{
			(mInstance->*transitionFunc)();
		}
		mLastState = mCurrState;
		mCurrState = mTransitions[mPendingTransition].to;
		mPendingTransition = NO_TRANSITION;
		mTimeInState = 0.0f;
	}
	else
	{
		mTimeInState += dt;
	}

	TickFunc tickFunc = mStates[mCurrState];
	(mInstance->*tickFunc)(dt);
}
