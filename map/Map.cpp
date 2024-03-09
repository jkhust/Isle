

// // ------------------------------------------------------------------------------------
// // not entirely accurate due to 1.0f - step, but good enough for a block-based world.
// Block * Map::rayTestBlocks( const Vec3f& p, const Vec3f& q )
// {
// 	float step = fmin( 4.0f / Vec3f::distance( p, q ), 1.0f );

// 	Vec3f pos;
// 	for( float t=step; t <= (1.0f - step); t += step )
// 	{
// 		pos = Vec3f::lerp( p, q, t );
// 		Vec3i loc = xyzToIJK( pos.x, pos.y, pos.z );

// 		// early out if we ever go OOB
// 		if( !isLocInMapBounds( loc.i, loc.j, loc.k ) ) return NULL;

// 		Block * block = blockAtIJK( loc.i, loc.j, loc.k );
// 		if( block )
// 		{
// 			return block;
// 		}
// 	}

// 	return NULL;
// }

// // ------------------------------------------------------------------------------------
// void Map::updateOcclusion( Patch *p )
// {
// 	int samples = 0;
// 	int free = 0;

// 	while( samples < mOcclusionSamples )
// 	{
// 		Vec3f dir = Vec3f::cosWeightedHemisphere( *(p->baseNormal) );
// 		dir.normalize();

// 		Vec3f end( dir );
// 		end.scale( 9999.0f );
// 		end.add( p->center );

// 		Vec3f start( dir );
// 		start.scale( 0.01 );
// 		start.add( p->center );

// 		++samples;
// 		if( !rayTestBlocks( start, end ) )
// 		{
// 			++free;
// 		}
// 	}

// 	p->occlusion = (float) free / samples;
// 	// printf("occlusion %d out of %d is %f   with normal %f %f %f\n", free, samples, p->occlusion, p->baseNormal->x, p->baseNormal->y, p->baseNormal->z );
// }



// 	// *** indirect illum ***
// 	// for( patchVisData in patchVisData[p] )
// 	// {
// 	// 	Vec3f emittedColor( patchVisData->q->emitted[pass] );
// 	// 	Vec3f dotAtten = maxf( Vec3f::dot( p->normal, q->normal ), 0.0f ) * patchVisData->attenFromQ;
// 	// 	emittedColor.scale( dotAtten );
// 	// 	illum.add( emittedColor );
// 	// }

// 	// 3) *** TOTAL ***
// 	// increment total illum by direct plus indirect
// 	// p->illum.add( illum );


// 	// 4) *** EMITTED ***
// 	// "split" the emitted out of p on the next frame.
// 	// illum.scale( ( p->reflectance * REFLECTION_COEFF ) / patchVisData[p].size );
// 	// p->emitted[nextPass].set( illum );
// }

// // ------------------------------------------------------------------------------------
// void Map::render()
// {
// 	light();

// 	glMatrixMode( GL_MODELVIEW );


// }

// // ------------------------------------------------------------------------------------
// void Map::visPatches()
// {
// 	printf("Revis %lu patches:\t", mOpenPatches.size() );

// 	Uint32 beforeTicks = SDL_GetTicks();

// 	mPatchPaths.clear();

// 	for( int i=0; i < mOpenPatches.size(); ++i )
// 	{
// 		Patch *p = mOpenPatches[i];

// 		for( int j=i; j < mOpenPatches.size(); ++j )
// 		{
// 			Patch *q = mOpenPatches[j];

// 		//	printf("testing %d %d\n",i, j );

// 			// facing eachother (using non-perturbed normals), and a free path between?
// 			float pDotQ = Vec3f::dot( (*p->baseNormal), (*q->baseNormal) );
// 			if( pDotQ <= 0.0f && !rayTestBlocks( p->center, q->center ) )
// 			{
// 				float distAtten = Vec3f::distAtten( p->center, q->center );
// 				mPatchPaths[p].push_back( PatchVisData( q, distAtten ) );
// 				mPatchPaths[q].push_back( PatchVisData( p, distAtten ) );
// 			}
// 		}

// 		updateOcclusion( p );
// 	}

// 	Uint32 afterTicks = SDL_GetTicks();
// 	printf("%0.4f\n", (float) ((afterTicks - beforeTicks) / 1000.0f ) );

// 	visLights();
// }

// // ------------------------------------------------------------------------------------
// void Map::visLights()
// {
// 	printf("Revis lights to %lu patches:\t", mOpenPatches.size() );

// 	Uint32 beforeTicks = SDL_GetTicks();

// 	mLightPaths.clear();

// 	for( int i=0; i < mOpenPatches.size(); ++i )
// 	{
// 		Patch *p = mOpenPatches[i];

// 		// TODO: make this more obvious that we are skipping ambient light #1.
// 		for( int j=1; j < MAX_LIGHTS; ++j )
// 		{
// 			Light* l = &mLights[j];

// 			Vec3f color;
// 			if( l->colorAt( this, p, color ) )
// 			{
// 				mLightPaths[p].push_back( LightVisData(l, color ) );
// 			}
// 		}
// 	}

// 	Uint32 afterTicks = SDL_GetTicks();
// 	printf("%0.4f\n", (float) ((afterTicks - beforeTicks) / 1000.0f ) );
// }

