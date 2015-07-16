#include "Kinect.h"
#include "OgreVector2.h"
#include "OgreMath.h"
#include "OgreOverlay.h"
#include "OgreOverlayManager.h"
#include "OgreOverlayContainer.h"
#include "Logger.h"


Kinect::Kinect(void)
{
	mToso1Overlay = Ogre::OverlayManager::getSingleton().getByName("Kinect/Torso1");
	mToso2Overlay = Ogre::OverlayManager::getSingleton().getByName("Kinect/Torso2");
	mCallibrationText = Ogre::OverlayManager::getSingleton().getOverlayElement("Kinect/Calibrate2");

	mCallibrating = false;

	mCallibrationOverlay = Ogre::OverlayManager::getSingleton().getByName("Kinect/CallibrationOverlay");

	mTimeSinceLastUpdate = 5.0; // Start out uncallibrated ...

	mToso1Overlay->show();
	mToso2Overlay->show();

	mToso1Overlay->setScroll(0.85f, 0.8f);
	mToso2Overlay->setScroll(0.65f, 0.8f);

	mXLeftMinDif = -0.572413862f; 
	mXLeftMaxDif = 0.191661924f;
	mYLeftMinDif = -0.426941812f;
	mYLeftMaxDif = 0.335171402f;

	mXRightMinDif = -0.217648357f;
	mXRightMaxDif = 0.530554414f;
	mYRightMinDif = -0.382603377f;
	mYRightMaxDif = 0.276229203;

	baseVectorDelta = Ogre::Vector2::ZERO;

	mCallibrated = true;

	mLeftWristPos = Ogre::Vector3(0,mYLeftMinDif,0);
	mRightWristPos = Ogre::Vector3(0,mYRightMinDif,0);
	mCenterPos = Ogre::Vector3::ZERO;
}


void
Kinect::addSkelListener(KinectSkelMsgr *listener)
{
  mSkelListeners.push_back(listener);
}


Kinect::~Kinect(void)
{
}

HRESULT
Kinect::initSensor()
{
#ifdef KINECT_AVAILABLE
	HRESULT  hr;

	if ( !m_pNuiSensor  || true)
	{

		HRESULT hr = NuiCreateSensorByIndex(0, &m_pNuiSensor);

		if ( FAILED(hr) )
		{
			return hr;
		}

		m_instanceId = m_pNuiSensor->NuiDeviceConnectionId();
	}

	//DWORD nuiFlags = NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX | NUI_INITIALIZE_FLAG_USES_SKELETON |  NUI_INITIALIZE_FLAG_USES_COLOR;
	DWORD nuiFlags =  NUI_INITIALIZE_FLAG_USES_SKELETON;
	hr = m_pNuiSensor->NuiInitialize( nuiFlags );


	if ( FAILED( hr ) )
	{
		return hr;
	}

	if ( HasSkeletalEngine( m_pNuiSensor ) )
	{
		m_hNextSkeletonEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

//		hr = m_pNuiSensor->NuiSkeletonTrackingEnable( m_hNextSkeletonEvent, NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT );
		hr = m_pNuiSensor->NuiSkeletonTrackingEnable( m_hNextSkeletonEvent, 0 );
		if( FAILED( hr ) )
		{
			return hr;
		}
	}

	m_hEvNuiProcessStop = CreateEvent( NULL, FALSE, FALSE, NULL );
	m_hThNuiProcess = CreateThread( NULL, 0, Nui_ProcessThread, this, 0, NULL );
	return hr;
#else
	return 0;
#endif

}


void Kinect::StartSession()
{
	mSessionStarted = true;
	mTimeSinceLastLog = 0.0f;

}
void Kinect::EndSession()
{
	mSessionStarted = false;

}

Ogre::Vector3 Kinect::handPositionAdjusted(bool &usingLeft) 
{
	float xPercent;
	float yPercent;
	if (mCallibrated)
	{
		if ( mLeftWristPos.y > mRightWristPos.y)
		{
			usingLeft = true;
			xPercent =  (mLeftWristPos.x - mCenterPos.x - mXLeftMinDif) / (mXLeftMaxDif - mXLeftMinDif);
			yPercent =  (mLeftWristPos.y - mCenterPos.y - mYLeftMinDif) / (mYLeftMaxDif - mYLeftMinDif);
		}
		else
		{
			usingLeft = false;
			xPercent =  (mRightWristPos.x - mCenterPos.x - mXRightMinDif) / (mXRightMaxDif - mXRightMinDif);
			yPercent =  (mRightWristPos.y - mCenterPos.y - mYRightMinDif) / (mYRightMaxDif - mYRightMinDif);

		}
		xPercent = Ogre::Math::Clamp<float>(xPercent,0,1);
		yPercent = Ogre::Math::Clamp<float>(yPercent,0,1);
		return Ogre::Vector3(xPercent, yPercent, 0);

	}
}
void 
Kinect::callibrate(float delay, std::function<void(void)> callback)
{
    mCallibrationFinishedCallback = callback;

	if (delay == 0)
	{
		recenterNext = true;
	}
	else
	{
		std::string message = "Callibration in ";

		long long printDelay = (long long) (delay + 0.99f);

		message.append(std::to_string(printDelay));

		message.append(" seconds");

		mCallibrationText->setCaption(message);
		mCalibrationClock = delay;
		mCallibrating = true;
		mCallibrationOverlay->show();
	}
}

void
Kinect::cancelCallibration()
{
    mCallibrating = false;
	mCallibrationFinishedCallback();
    // mCallibrationFinishedCallback = NULL; // Note:  Why doesn't this work?
    mCallibrationOverlay->hide();
   
}
void
Kinect::update(float time)
{
	mTimeSinceLastUpdate += time;
	if (mCallibrating)
	{
		if (mTimeSinceLastUpdate > 1.0)
		{
			mCallibrationText->setCaption("Kinect cannot see you. \nPlease adjust position");
		}
		else
		{

			mCalibrationClock -= time;
			if (mCalibrationClock <= 0)
			{
                if (mCallibrationFinishedCallback != NULL)
                {
                    mCallibrationFinishedCallback();
                }
				recenterNext = true;
				mCallibrating = false;	
				mCallibrationOverlay->hide();
			}
			else
			{

				std::string message = "Callibration in ";

				long long printDelay = (long long) (mCalibrationClock + 0.99f);

				message.append(std::to_string(printDelay));

				message.append(" seconds");

				mCallibrationText->setCaption(message);
			}

		}
	}
	else if(mSessionStarted)
	{
		mTimeSinceLastLog += time;
		if(mTimeSinceLastLog >= Logger::getInstance()->getTimeStep())
		{
			float lr, fb, lrt;
			for (std::vector<KinectSkelMsgr *>::iterator it = mSkelListeners.begin(); it != mSkelListeners.end(); it++)
			{
				lr = mLeftRightAngle.valueDegrees();
				fb = mFrontBackAngle.valueDegrees();
				lrt = mLeftRightTrue.valueDegrees();
				(*it)->ReceiveSkelData(new SkelData(lr, fb, lrt, mSkelPositions));
			} 
			mTimeSinceLastLog = 0.0;
		}
	}
}





void
Kinect::updateKinectSkeleton()
{
#ifdef KINECT_AVAILABLE
	NUI_SKELETON_FRAME SkeletonFrame = {0};

	bool bFoundSkeleton = false;

	if ( SUCCEEDED(m_pNuiSensor->NuiSkeletonGetNextFrame( 0, &SkeletonFrame )) )
	{
		for ( int i = 0 ; i < NUI_SKELETON_COUNT ; i++ )
		{
			if( SkeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED ||
				(SkeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_POSITION_ONLY)) 
			{
				bFoundSkeleton = true;
			}
		}
	}

	// no skeletons!
	if( !bFoundSkeleton )
	{
		return;
	}
	mTimeSinceLastUpdate = 0;


	// smooth out the skeleton data
	HRESULT hr = m_pNuiSensor->NuiTransformSmooth(&SkeletonFrame,NULL);
	if ( FAILED(hr) )
	{
		return;
	}

	int x = 0;

	bool bSkeletonIdsChanged = false;
	for ( int i = 0 ; i < NUI_SKELETON_COUNT; i++ )
	{
		if ( m_SkeletonIds[i] != SkeletonFrame.SkeletonData[i].dwTrackingID )
		{
			m_SkeletonIds[i] = SkeletonFrame.SkeletonData[i].dwTrackingID;
			bSkeletonIdsChanged = true;
		}

		// Show skeleton only if it is tracked, and the center-shoulder joint is at least inferred.
		if ( SkeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED &&
			SkeletonFrame.SkeletonData[i].eSkeletonPositionTrackingState[NUI_SKELETON_POSITION_SHOULDER_CENTER] != NUI_SKELETON_POSITION_NOT_TRACKED)
		{
			// Here's our skeleton, let's update it.  Multiple tracked skeletons could be a problem,
			// fix that later

			NUI_SKELETON_DATA * pSkel =  &SkeletonFrame.SkeletonData[i];

			// TODO:  Check for     pSkel->eSkeletonPositionTrackingState[ JOINT ];
			Vector4 shoulderPos = pSkel->SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_CENTER];
			Vector4 headPos = pSkel->SkeletonPositions[NUI_SKELETON_POSITION_HEAD];
			Vector4 leftShoulder =  pSkel->SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_LEFT];
			Vector4 rightShoulder =  pSkel->SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_RIGHT];
			Vector4 leftElbow =  pSkel->SkeletonPositions[NUI_SKELETON_POSITION_ELBOW_LEFT];
			Vector4 rightElbow =  pSkel->SkeletonPositions[NUI_SKELETON_POSITION_ELBOW_RIGHT];
			Vector4 leftWrist =  pSkel->SkeletonPositions[NUI_SKELETON_POSITION_WRIST_LEFT];
			Vector4 rightWrist =  pSkel->SkeletonPositions[NUI_SKELETON_POSITION_WRIST_RIGHT];
			Vector4 leftHand =  pSkel->SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT];
			Vector4 rightHand =  pSkel->SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT];
			Vector4 hip = pSkel->SkeletonPositions[NUI_SKELETON_POSITION_HIP_CENTER];

			for (int i = 0 ; i < NUI_SKELETON_POSITION_COUNT; i++)
			{
				mSkelPositions[i] = Ogre::Vector3(pSkel->SkeletonPositions[i].x,pSkel->SkeletonPositions[i].y,pSkel->SkeletonPositions[i].z);
			}


			int x = 3;
			Ogre::Vector2 leftVector(leftShoulder.x - rightShoulder.x, leftShoulder.z - rightShoulder.z);
			leftVector.normalise();
			Ogre::Vector2 FrontVector(leftVector.y, -leftVector.x);

			//Ogre::Vector2 BaseVector((leftElbow.x + rightElbow.x + leftShoulder.x + rightShoulder.x) / 4,
			//	(leftElbow.z + rightElbow.z + leftShoulder.z + rightShoulder.z) / 4);

			Ogre::Vector2 BaseVector(hip.x, hip.z);


			if (recenterNext)
			{
				recenterNext = false;
				baseVectorDelta = Ogre::Vector2(headPos.x, headPos.z) - BaseVector;
			}

			// Note:  headPos is a 3D point, baseVector is a 2D point, hence z/y confusion
			float xDisplacement = (headPos.x - BaseVector.x - baseVectorDelta.x) * leftVector.x + (headPos.z - BaseVector.y-baseVectorDelta.y) * leftVector.y;

			float xDisplacement2 = (headPos.x - BaseVector.x) * leftVector.x + (headPos.z - BaseVector.y) * leftVector.y;

			Ogre::Radian leftRightAngle1 = Ogre::Math::ATan2(-xDisplacement, headPos.y - shoulderPos.y + 0.5f);
			Ogre::Radian leftRightAngle2 = Ogre::Math::ATan2(-xDisplacement2, headPos.y - shoulderPos.y + 0.5f);
			mLeftRightAngle = leftRightAngle1 * 4;
			mLeftRightTrue = leftRightAngle2 * 4;

			float ZDisplacement = (headPos.x - BaseVector.x-baseVectorDelta.x) * FrontVector.x + (headPos.z - BaseVector.y-baseVectorDelta.y) * FrontVector.y;

			Ogre::Radian frontBackAngle1 = Ogre::Math::ATan2(ZDisplacement, headPos.y - shoulderPos.y + 0.5f);
			mFrontBackAngle = frontBackAngle1 * 4;

			mLeftWristPos = Ogre::Vector3(leftWrist.x, leftWrist.y, leftWrist.z);
			mRightWristPos = Ogre::Vector3(rightWrist.x, rightWrist.y, rightWrist.z);
			mCenterPos = Ogre::Vector3(shoulderPos.x, shoulderPos.y, shoulderPos.z);

		}
		else if ( true && SkeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_POSITION_ONLY )
		{
			// Position only, ignore for now.
		}
	}
	//mToso1Overlay->setRotate(Ogre::Radian(-mLeftRightAngle));

	mToso1Overlay->setRotate(Ogre::Radian(-mLeftRightAngle));
	mToso1Overlay->setScroll(0.85f, 0.8f);

	mToso2Overlay->setRotate(Ogre::Radian(-mFrontBackAngle));
	mToso2Overlay->setScroll(0.65f, 0.8f);
#endif
}

#ifdef KINECT_AVAILABLE
DWORD WINAPI Kinect::Nui_ProcessThread(LPVOID pParam)
{
	Kinect *pthis = (Kinect *) pParam;
	return pthis->Nui_ProcessThread();
}
#endif 

void
Kinect::shutdown()
{
#ifdef KINECT_AVAILABLE
	    // Stop the Nui processing thread
    if ( NULL != m_hEvNuiProcessStop )
    {
        // Signal the thread
        SetEvent(m_hEvNuiProcessStop);

        // Wait for thread to stop
        if ( NULL != m_hThNuiProcess )
        {
            WaitForSingleObject( m_hThNuiProcess, INFINITE );
            CloseHandle( m_hThNuiProcess );
        }
        CloseHandle( m_hEvNuiProcessStop );
    }

    if ( m_pNuiSensor )
    {
        m_pNuiSensor->NuiShutdown( );
    }
    if ( m_hNextSkeletonEvent && ( m_hNextSkeletonEvent != INVALID_HANDLE_VALUE ) )
    {
        CloseHandle( m_hNextSkeletonEvent );
        m_hNextSkeletonEvent = NULL;
    }

    if ( m_pNuiSensor )
    {
        m_pNuiSensor->Release();
        m_pNuiSensor = NULL;
    }
#endif

}

#ifdef KINECT_AVAILABLE
//-------------------------------------------------------------------
// Nui_ProcessThread
//
// Thread to handle Kinect processing
//-------------------------------------------------------------------
DWORD WINAPI Kinect::Nui_ProcessThread()
{
	const int numEvents = 2;
	HANDLE hEvents[numEvents] = { m_hEvNuiProcessStop, m_hNextSkeletonEvent };
	int    nEventIdx;


	// Main thread loop
	bool continueProcessing = true;
	while ( continueProcessing )
	{
		// Wait for any of the events to be signalled
		nEventIdx = WaitForMultipleObjects( numEvents, hEvents, FALSE, 5000 );

		// Process signal events
		switch ( nEventIdx )
		{
		case WAIT_TIMEOUT:
//			continueProcessing = false;
			continue;

			// If the stop event, stop looping and exit
		case WAIT_OBJECT_0:
			continueProcessing = false;
			continue;

		case WAIT_OBJECT_0 + 1:
			updateKinectSkeleton( );
			break;
		}

	}

	return 0;
}
#endif