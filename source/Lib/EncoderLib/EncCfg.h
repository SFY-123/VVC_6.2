/* The copyright in this software is being made available under the BSD
 * License, included below. This software may be subject to other third party
 * and contributor rights, including patent rights, and no such rights are
 * granted under this license.
 *
 * Copyright (c) 2010-2019, ITU/ISO/IEC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  * Neither the name of the ITU/ISO/IEC nor the names of its contributors may
 *    be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

/** \file     EncCfg.h
    \brief    encoder configuration class (header)
*/

#ifndef __ENCCFG__
#define __ENCCFG__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CommonLib/CommonDef.h"
#include "CommonLib/Slice.h"

#include "CommonLib/Unit.h"

#if JVET_O0756_CALCULATE_HDRMETRICS
#include "HDRLib/inc/DistortionMetric.H"
#endif

struct GOPEntry
{
  int m_POC;
  int m_QPOffset;
#if X0038_LAMBDA_FROM_QP_CAPABILITY
  double m_QPOffsetModelOffset;
  double m_QPOffsetModelScale;
#endif
#if W0038_CQP_ADJ
  int m_CbQPoffset;
  int m_CrQPoffset;
#endif
  double m_QPFactor;
  int m_tcOffsetDiv2;
  int m_betaOffsetDiv2;
  int m_temporalId;
  bool m_refPic;
  int8_t m_sliceType;
  int m_numRefPicsActive0;
  int m_numRefPics0;
  int m_deltaRefPics0[MAX_NUM_REF_PICS];
  int m_numRefPicsActive1;
  int m_numRefPics1;
  int m_deltaRefPics1[MAX_NUM_REF_PICS];
  bool m_isEncoded;
#if JVET_N0100_PROPOSAL1
  bool m_ltrp_in_slice_header_flag;
#endif
  GOPEntry()
  : m_POC(-1)
  , m_QPOffset(0)
#if X0038_LAMBDA_FROM_QP_CAPABILITY
  , m_QPOffsetModelOffset(0)
  , m_QPOffsetModelScale(0)
#endif
#if W0038_CQP_ADJ
  , m_CbQPoffset(0)
  , m_CrQPoffset(0)
#endif
  , m_QPFactor(0)
  , m_tcOffsetDiv2(0)
  , m_betaOffsetDiv2(0)
  , m_temporalId(0)
  , m_refPic(false)
  , m_sliceType('P')
    , m_numRefPicsActive0(0)
    , m_numRefPics0(0)
    , m_numRefPicsActive1(0)
    , m_numRefPics1(0)
    , m_isEncoded(false)
#if JVET_N0100_PROPOSAL1
    , m_ltrp_in_slice_header_flag(false)
#endif
  {
    ::memset(m_deltaRefPics0, 0, sizeof(m_deltaRefPics0));
    ::memset(m_deltaRefPics1, 0, sizeof(m_deltaRefPics1));
  }
};

struct RPLEntry
{
  int m_POC;
  int m_temporalId;
  bool m_refPic;
  int m_numRefPicsActive;
  int8_t m_sliceType;
  int m_numRefPics;
  int m_deltaRefPics[MAX_NUM_REF_PICS];
  bool m_isEncoded;
#if JVET_N0100_PROPOSAL1
  bool m_ltrp_in_slice_header_flag;
#endif
  RPLEntry()
    : m_POC(-1)
    , m_temporalId(0)
    , m_refPic(false)
    , m_numRefPicsActive(0)
    , m_sliceType('P')
    , m_numRefPics(0)
    , m_isEncoded(false)
#if JVET_N0100_PROPOSAL1
    , m_ltrp_in_slice_header_flag(false)
#endif
  {
    ::memset(m_deltaRefPics, 0, sizeof(m_deltaRefPics));
  }
};

std::istringstream &operator>>(std::istringstream &in, GOPEntry &entry);     //input

struct BrickSplit
{
  int     m_tileIdx;
  bool    m_uniformSplit;
  int     m_uniformHeight;
  int     m_numSplits;
  int     m_brickHeight[MAX_NUM_BRICKS_PER_TILE];
  BrickSplit()
    : m_tileIdx(-1)
    , m_uniformSplit(true)
    , m_uniformHeight(0)
    , m_numSplits(0)
  {
    ::memset( m_brickHeight, 0, sizeof(m_brickHeight) );
  }
};

typedef std::map<int, BrickSplit> BrickSplitMap;

std::istringstream &operator>>(std::istringstream &in, BrickSplit &entry);     //input


//! \ingroup EncoderLib
//! \{

// ====================================================================================================================
// Class definition
// ====================================================================================================================

/// encoder configuration class
class EncCfg
{
protected:
  //==== File I/O ========
  int       m_iFrameRate;
  int       m_FrameSkip;
  uint32_t      m_temporalSubsampleRatio;
  int       m_iSourceWidth;
  int       m_iSourceHeight;
  Window    m_conformanceWindow;
  int       m_framesToBeEncoded;
  double    m_adLambdaModifier[ MAX_TLAYER ];
  std::vector<double> m_adIntraLambdaModifier;
  double    m_dIntraQpFactor;                                 ///< Intra Q Factor. If negative, use a default equation: 0.57*(1.0 - Clip3( 0.0, 0.5, 0.05*(double)(isField ? (GopSize-1)/2 : GopSize-1) ))

  bool      m_printMSEBasedSequencePSNR;
  bool      m_printHexPsnr;
  bool      m_printFrameMSE;
  bool      m_printSequenceMSE;
  bool      m_cabacZeroWordPaddingEnabled;

  bool      m_bIntraOnlyConstraintFlag;
  uint32_t  m_maxBitDepthConstraintIdc;
  uint32_t  m_maxChromaFormatConstraintIdc;
  bool      m_bFrameConstraintFlag;
  bool      m_bNoQtbttDualTreeIntraConstraintFlag;
  bool      m_noPartitionConstraintsOverrideConstraintFlag;
  bool      m_bNoSaoConstraintFlag;
  bool      m_bNoAlfConstraintFlag;
#if !JVET_O0525_REMOVE_PCM
  bool      m_bNoPcmConstraintFlag;
#endif
  bool      m_bNoRefWraparoundConstraintFlag;
  bool      m_bNoTemporalMvpConstraintFlag;
  bool      m_bNoSbtmvpConstraintFlag;
  bool      m_bNoAmvrConstraintFlag;
  bool      m_bNoBdofConstraintFlag;
  bool      m_noDmvrConstraintFlag;
  bool      m_bNoCclmConstraintFlag;
  bool      m_bNoMtsConstraintFlag;
  bool      m_noSbtConstraintFlag;
  bool      m_bNoAffineMotionConstraintFlag;
  bool      m_bNoGbiConstraintFlag;
  bool      m_noIbcConstraintFlag;
  bool      m_bNoMhIntraConstraintFlag;
  bool      m_noFPelMmvdConstraintFlag;
  bool      m_bNoTriangleConstraintFlag;
  bool      m_bNoLadfConstraintFlag;
  bool      m_noTransformSkipConstraintFlag;
#if JVET_O1136_TS_BDPCM_SIGNALLING
  bool      m_noBDPCMConstraintFlag;
#endif
#if JVET_O0376_SPS_JOINTCBCR_FLAG
  bool      m_noJointCbCrConstraintFlag;
#endif
  bool      m_bNoQpDeltaConstraintFlag;
  bool      m_bNoDepQuantConstraintFlag;
  bool      m_bNoSignDataHidingConstraintFlag;

  /* profile & level */
  Profile::Name m_profile;
  Level::Tier   m_levelTier;
  Level::Name   m_level;
#if JVET_O0044_MULTI_SUB_PROFILE
  std::vector<uint32_t>      m_subProfile;
  uint8_t       m_numSubProfile;
#else
  uint32_t      m_subProfile;
#endif
  bool m_progressiveSourceFlag;
  bool m_interlacedSourceFlag;
  bool m_nonPackedConstraintFlag;
  bool m_frameOnlyConstraintFlag;
  uint32_t              m_bitDepthConstraintValue;
  ChromaFormat      m_chromaFormatConstraintValue;
  bool              m_intraConstraintFlag;
  bool              m_onePictureOnlyConstraintFlag;
  bool              m_lowerBitRateConstraintFlag;

  //====== Coding Structure ========
  int       m_uiIntraPeriod;                        // needs to be signed to allow '-1' for no intra period
  uint32_t      m_uiDecodingRefreshType;            ///< the type of decoding refresh employed for the random access.
  bool      m_rewriteParamSets;
  bool      m_idrRefParamList;
  int       m_iGOPSize;
  RPLEntry  m_RPLList0[MAX_GOP];
  RPLEntry  m_RPLList1[MAX_GOP];
  int       m_numRPLList0;
  int       m_numRPLList1;
  GOPEntry  m_GOPList[MAX_GOP];
  int       m_maxDecPicBuffering[MAX_TLAYER];
  int       m_numReorderPics[MAX_TLAYER];
#if JVET_N0494_DRAP
  int       m_drapPeriod;
#endif

  int       m_iQP;                              //  if (AdaptiveQP == OFF)
#if JVET_O0650_SIGNAL_CHROMAQP_MAPPING_TABLE
  ChromaQpMappingTableParams m_chromaQpMappingTableParams;
#endif
#if X0038_LAMBDA_FROM_QP_CAPABILITY
  int       m_intraQPOffset;                    ///< QP offset for intra slice (integer)
  int       m_lambdaFromQPEnable;               ///< enable lambda derivation from QP
#endif
  int       m_aiPad[2];

  bool      m_AccessUnitDelimiter;               ///< add Access Unit Delimiter NAL units

  int       m_iMaxRefPicNum;                     ///< this is used to mimic the sliding mechanism used by the decoder
                                                 // TODO: We need to have a common sliding mechanism used by both the encoder and decoder

  int       m_maxTempLayer;                      ///< Max temporal layer
  unsigned  m_CTUSize;
  bool      m_useSplitConsOverride;
  unsigned  m_uiMinQT[3]; //0: I slice; 1: P/B slice, 2: I slice chroma
  unsigned  m_uiMaxMTTHierarchyDepth;
  unsigned  m_uiMaxMTTHierarchyDepthI;
  unsigned  m_uiMaxMTTHierarchyDepthIChroma;
  bool      m_dualITree;
  unsigned  m_maxCUWidth;
  unsigned  m_maxCUHeight;
  unsigned  m_maxTotalCUDepth;
  unsigned  m_log2DiffMaxMinCodingBlockSize;

  int       m_LMChroma;
  bool      m_cclmCollocatedChromaFlag;
  int       m_IntraMTS;
  int       m_InterMTS;
  int       m_IntraMTSMaxCand;
  int       m_InterMTSMaxCand;
  int       m_ImplicitMTS;
  bool      m_SBT;                                ///< Sub-Block Transform for inter blocks
  bool      m_LFNST;
  bool      m_useFastLFNST;
  int       m_SubPuMvpMode;
  bool      m_Affine;
  bool      m_AffineType;
#if JVET_O0070_PROF
  bool      m_PROF;
#endif
  bool      m_BIO;

  bool      m_SMVD;
  bool      m_compositeRefEnabled;        //composite reference
  bool      m_GBi;
  bool      m_GBiFast;
#if LUMA_ADAPTIVE_DEBLOCKING_FILTER_QP_OFFSET
  bool      m_LadfEnabled;
  int       m_LadfNumIntervals;
  int       m_LadfQpOffset[MAX_LADF_INTERVALS];
  int       m_LadfIntervalLowerBound[MAX_LADF_INTERVALS];
#endif

  bool      m_MHIntra;
  bool      m_Triangle;
  bool      m_allowDisFracMMVD;
  bool      m_AffineAmvr;
  bool      m_HashME;
  bool      m_AffineAmvrEncOpt;
  bool      m_DMVR;
  bool      m_MMVD;
  int       m_MmvdDisNum;
#if !JVET_O1136_TS_BDPCM_SIGNALLING  
  bool      m_RdpcmMode;
#endif  
#if JVET_O0119_BASE_PALETTE_444
  unsigned  m_PLTMode;
#endif
#if JVET_O0376_SPS_JOINTCBCR_FLAG
  bool      m_JointCbCrMode;
#endif
  unsigned  m_IBCMode;
  unsigned  m_IBCLocalSearchRangeX;
  unsigned  m_IBCLocalSearchRangeY;
  unsigned  m_IBCHashSearch;
  unsigned  m_IBCHashSearchMaxCand;
  unsigned  m_IBCHashSearchRange4SmallBlk;
  unsigned  m_IBCFastMethod;

  bool      m_wrapAround;
  unsigned  m_wrapAroundOffset;

  // ADD_NEW_TOOL : (encoder lib) add tool enabling flags and associated parameters here
  bool      m_loopFilterAcrossVirtualBoundariesDisabledFlag;
  unsigned  m_numVerVirtualBoundaries;
  unsigned  m_numHorVirtualBoundaries;
  unsigned  m_virtualBoundariesPosX[3];
  unsigned  m_virtualBoundariesPosY[3];
  bool      m_lumaReshapeEnable;
  unsigned  m_reshapeSignalType;
  unsigned  m_intraCMD;
  ReshapeCW m_reshapeCW;
  bool      m_encDbOpt;
  bool      m_useFastLCTU;
  bool      m_useFastMrg;
  bool      m_usePbIntraFast;
  bool      m_useAMaxBT;
  bool      m_e0023FastEnc;
  bool      m_contentBasedFastQtbt;
  bool      m_useNonLinearAlfLuma;
  bool      m_useNonLinearAlfChroma;
#if JVET_O0090_ALF_CHROMA_FILTER_ALTERNATIVES_CTB
  unsigned  m_maxNumAlfAlternativesChroma;
#endif
  bool      m_MIP;
  bool      m_useFastMIP;
#if JVET_O0050_LOCAL_DUAL_TREE
  int       m_fastLocalDualTreeMode;
#endif
#if MAX_TB_SIZE_SIGNALLING
  uint32_t  m_log2MaxTbSize;
#endif

  //====== Loop/Deblock Filter ========
  bool      m_bLoopFilterDisable;
  bool      m_loopFilterOffsetInPPS;
  int       m_loopFilterBetaOffsetDiv2;
  int       m_loopFilterTcOffsetDiv2;
#if W0038_DB_OPT
  int       m_deblockingFilterMetric;
#else
  bool      m_DeblockingFilterMetric;
#endif
  bool      m_bUseSAO;
  bool      m_bTestSAODisableAtPictureLevel;
  double    m_saoEncodingRate;       // When non-0 SAO early picture termination is enabled for luma and chroma
  double    m_saoEncodingRateChroma; // The SAO early picture termination rate to use for chroma (when m_SaoEncodingRate is >0). If <=0, use results for luma.
  int       m_maxNumOffsetsPerPic;
  bool      m_saoCtuBoundary;

  bool      m_saoGreedyMergeEnc;
  //====== Motion search ========
  bool      m_bDisableIntraPUsInInterSlices;
  MESearchMethod m_motionEstimationSearchMethod;
  int       m_iSearchRange;                     //  0:Full frame
  int       m_bipredSearchRange;
  bool      m_bClipForBiPredMeEnabled;
  bool      m_bFastMEAssumingSmootherMVEnabled;
  int       m_minSearchWindow;
  bool      m_bRestrictMESampling;

  //====== Quality control ========
  int       m_iMaxDeltaQP;                      //  Max. absolute delta QP (1:default)
  int       m_cuQpDeltaSubdiv;                  //  Max. subdivision level for a CuDQP (0:default)
  int       m_cuChromaQpOffsetSubdiv;           ///< If negative, then do not apply chroma qp offsets.

  int       m_chromaCbQpOffset;                 //  Chroma Cb QP Offset (0:default)
  int       m_chromaCrQpOffset;                 //  Chroma Cr Qp Offset (0:default)
  int       m_chromaCbQpOffsetDualTree;         //  Chroma Cb QP Offset for dual tree
  int       m_chromaCrQpOffsetDualTree;         //  Chroma Cr Qp Offset for dual tree
  int       m_chromaCbCrQpOffset;               //  QP Offset for the joint Cb-Cr mode
  int       m_chromaCbCrQpOffsetDualTree;       //  QP Offset for the joint Cb-Cr mode in dual tree
#if ER_CHROMA_QP_WCG_PPS
  WCGChromaQPControl m_wcgChromaQpControl;                    ///< Wide-colour-gamut chroma QP control.
#endif
#if W0038_CQP_ADJ
  uint32_t      m_sliceChromaQpOffsetPeriodicity;                 ///< Used in conjunction with Slice Cb/Cr QpOffsetIntraOrPeriodic. Use 0 (default) to disable periodic nature.
  int       m_sliceChromaQpOffsetIntraOrPeriodic[2/*Cb,Cr*/]; ///< Chroma Cb QP Offset at slice level for I slice or for periodic inter slices as defined by SliceChromaQPOffsetPeriodicity. Replaces offset in the GOP table.
#endif

  ChromaFormat m_chromaFormatIDC;

  bool      m_extendedPrecisionProcessingFlag;
  bool      m_highPrecisionOffsetsEnabledFlag;
  bool      m_bUseAdaptiveQP;
  int       m_iQPAdaptationRange;
#if ENABLE_QPA
  bool      m_bUsePerceptQPA;
  bool      m_bUseWPSNR;
#endif

  //====== Tool list ========
  int       m_inputBitDepth[MAX_NUM_CHANNEL_TYPE];         ///< bit-depth of input file
  int       m_bitDepth[MAX_NUM_CHANNEL_TYPE];
  bool      m_bUseASR;
  bool      m_bUseHADME;
  bool      m_useRDOQ;
  bool      m_useRDOQTS;
#if T0196_SELECTIVE_RDOQ
  bool      m_useSelectiveRDOQ;
#endif
  uint32_t      m_rdPenalty;
  FastInterSearchMode m_fastInterSearchMode;
  bool      m_bUseEarlyCU;
  bool      m_useFastDecisionForMerge;
  bool      m_bUseCbfFastMode;
  bool      m_useEarlySkipDetection;
  bool      m_crossComponentPredictionEnabledFlag;
  bool      m_reconBasedCrossCPredictionEstimate;
  uint32_t      m_log2SaoOffsetScale[MAX_NUM_CHANNEL_TYPE];
  bool      m_useTransformSkip;
  bool      m_useTransformSkipFast;
#if JVET_O1136_TS_BDPCM_SIGNALLING
  bool      m_useBDPCM;
#endif
  uint32_t      m_log2MaxTransformSkipBlockSize;
  bool      m_transformSkipRotationEnabledFlag;
  bool      m_transformSkipContextEnabledFlag;
  bool      m_persistentRiceAdaptationEnabledFlag;
  bool      m_cabacBypassAlignmentEnabledFlag;
  bool      m_rdpcmEnabledFlag[NUMBER_OF_RDPCM_SIGNALLING_MODES];
#if SHARP_LUMA_DELTA_QP
  LumaLevelToDeltaQPMapping m_lumaLevelToDeltaQPMapping; ///< mapping from luma level to delta QP.
#endif
  int*      m_aidQP;
  uint32_t      m_uiDeltaQpRD;
  bool      m_bFastDeltaQP;
  bool      m_ISP;
  bool      m_useFastISP;

  bool      m_bUseConstrainedIntraPred;
  bool      m_bFastUDIUseMPMEnabled;
  bool      m_bFastMEForGenBLowDelayEnabled;
  bool      m_bUseBLambdaForNonKeyLowDelayPictures;
#if !JVET_O0525_REMOVE_PCM
  bool      m_usePCM;
  int       m_PCMBitDepth[MAX_NUM_CHANNEL_TYPE];
  uint32_t      m_pcmLog2MaxSize;
  uint32_t      m_uiPCMLog2MinSize;
#endif
  //====== Slice ========
  SliceConstraint m_sliceMode;
  int       m_sliceArgument;
  //====== Dependent Slice ========
  SliceConstraint m_sliceSegmentMode;
  int       m_sliceSegmentArgument;
  bool      m_bLFCrossSliceBoundaryFlag;

#if !JVET_O0525_REMOVE_PCM
  bool      m_bPCMInputBitDepthFlag;
  bool      m_bPCMFilterDisableFlag;
#endif
  bool      m_intraSmoothingDisabledFlag;
  bool      m_loopFilterAcrossBricksEnabledFlag;
  bool      m_tileUniformSpacingFlag;
  int       m_iNumColumnsMinus1;
  int       m_iNumRowsMinus1;
#if JVET_O0143_BOTTOM_RIGHT_BRICK_IDX_DELTA
  int       m_uniformTileColsWidthMinus1;
  int       m_uniformTileRowHeightMinus1;
#endif
  std::vector<int> m_tileColumnWidth;
  std::vector<int> m_tileRowHeight;

  bool      m_entropyCodingSyncEnabledFlag;

  bool      m_rectSliceFlag;
  int       m_numSlicesInPicMinus1;
  std::vector<int> m_topLeftBrickIdx;
  std::vector<int> m_bottomRightBrickIdx;
  bool      m_loopFilterAcrossSlicesEnabledFlag;
  bool      m_signalledSliceIdFlag;
  int       m_signalledSliceIdLengthMinus1;
  std::vector<int> m_sliceId;
  BrickSplitMap m_brickSplitMap;

  HashType  m_decodedPictureHashSEIType;
  bool      m_bufferingPeriodSEIEnabled;
  bool      m_pictureTimingSEIEnabled;
#if JVET_O0041_FRAME_FIELD_SEI
  bool      m_frameFieldInfoSEIEnabled;
#endif
#if JVET_N0494_DRAP
  bool      m_dependentRAPIndicationSEIEnabled;
#endif
#if HEVC_SEI
  bool      m_recoveryPointSEIEnabled;
  bool      m_toneMappingInfoSEIEnabled;
  int       m_toneMapId;
  bool      m_toneMapCancelFlag;
  bool      m_toneMapPersistenceFlag;
  int       m_codedDataBitDepth;
  int       m_targetBitDepth;
  int       m_modelId;
  int       m_minValue;
  int       m_maxValue;
  int       m_sigmoidMidpoint;
  int       m_sigmoidWidth;
  int       m_numPivots;
  int       m_cameraIsoSpeedIdc;
  int       m_cameraIsoSpeedValue;
  int       m_exposureIndexIdc;
  int       m_exposureIndexValue;
  bool      m_exposureCompensationValueSignFlag;
  int       m_exposureCompensationValueNumerator;
  int       m_exposureCompensationValueDenomIdc;
  int       m_refScreenLuminanceWhite;
  int       m_extendedRangeWhiteLevel;
  int       m_nominalBlackLevelLumaCodeValue;
  int       m_nominalWhiteLevelLumaCodeValue;
  int       m_extendedWhiteLevelLumaCodeValue;
  int*      m_startOfCodedInterval;
  int*      m_codedPivotValue;
  int*      m_targetPivotValue;
  bool      m_framePackingSEIEnabled;
  int       m_framePackingSEIType;
  int       m_framePackingSEIId;
  int       m_framePackingSEIQuincunx;
  int       m_framePackingSEIInterpretation;
  bool      m_segmentedRectFramePackingSEIEnabled;
  bool      m_segmentedRectFramePackingSEICancel;
  int       m_segmentedRectFramePackingSEIType;
  bool      m_segmentedRectFramePackingSEIPersistence;
  int       m_displayOrientationSEIAngle;
  bool      m_temporalLevel0IndexSEIEnabled;
  bool      m_gradualDecodingRefreshInfoEnabled;
  int       m_noDisplaySEITLayer;
#endif
#if JVET_N0867_TEMP_SCAL_HRD
  bool      m_bpDeltasGOPStructure;
#endif
  bool      m_decodingUnitInfoSEIEnabled;
#if HEVC_SEI
  bool      m_SOPDescriptionSEIEnabled;
  bool      m_scalableNestingSEIEnabled;
  bool      m_tmctsSEIEnabled;
#endif
  bool      m_MCTSEncConstraint;
#if HEVC_SEI
  bool      m_timeCodeSEIEnabled;
  int       m_timeCodeSEINumTs;
  SEITimeSet   m_timeSetArray[MAX_TIMECODE_SEI_SETS];
  bool      m_kneeSEIEnabled;
  int       m_kneeSEIId;
  bool      m_kneeSEICancelFlag;
  bool      m_kneeSEIPersistenceFlag;
  int       m_kneeSEIInputDrange;
  int       m_kneeSEIInputDispLuminance;
  int       m_kneeSEIOutputDrange;
  int       m_kneeSEIOutputDispLuminance;
  int       m_kneeSEINumKneePointsMinus1;
  int*      m_kneeSEIInputKneePoint;
  int*      m_kneeSEIOutputKneePoint;
  std::string m_colourRemapSEIFileRoot;          ///< SEI Colour Remapping File (initialized from external file)
  SEIMasteringDisplay m_masteringDisplay;
#if U0033_ALTERNATIVE_TRANSFER_CHARACTERISTICS_SEI
  bool      m_alternativeTransferCharacteristicsSEIEnabled;
  uint8_t     m_preferredTransferCharacteristics;
#endif
  bool      m_greenMetadataInfoSEIEnabled;
  uint8_t     m_greenMetadataType;
  uint8_t     m_xsdMetricType;
#endif
  //====== Weighted Prediction ========
  bool      m_useWeightedPred;       //< Use of Weighting Prediction (P_SLICE)
  bool      m_useWeightedBiPred;    //< Use of Bi-directional Weighting Prediction (B_SLICE)
  WeightedPredictionMethod m_weightedPredictionMethod;
  uint32_t      m_log2ParallelMergeLevelMinus2;       ///< Parallel merge estimation region
  uint32_t      m_maxNumMergeCand;                    ///< Maximum number of merge candidates
  uint32_t      m_maxNumAffineMergeCand;              ///< Maximum number of affine merge candidates
  uint32_t      m_maxNumTriangleCand;
#if JVET_O0455_IBC_MAX_MERGE_NUM
  uint32_t      m_maxNumIBCMergeCand;                 ///< Max number of IBC merge candidates
#endif
  ScalingListMode m_useScalingListId;             ///< Using quantization matrix i.e. 0=off, 1=default, 2=file.
  std::string m_scalingListFileName;              ///< quantization matrix file name
  int       m_TMVPModeId;
#if JVET_O0238_PPS_OR_SLICE
  bool      m_constantSliceHeaderParamsEnabledFlag;
  int       m_PPSDepQuantEnabledIdc;
  int       m_PPSRefPicListSPSIdc0;
  int       m_PPSRefPicListSPSIdc1;
  int       m_PPSTemporalMVPEnabledIdc;
  int       m_PPSMvdL1ZeroIdc;
  int       m_PPSCollocatedFromL0Idc;
  uint32_t  m_PPSSixMinusMaxNumMergeCandPlus1;
  uint32_t  m_PPSFiveMinusMaxNumSubblockMergeCandPlus1;
  uint32_t  m_PPSMaxNumMergeCandMinusMaxNumTriangleCandPlus1;
#endif
  bool      m_DepQuantEnabledFlag;
  bool      m_SignDataHidingEnabledFlag;
  bool      m_RCEnableRateControl;
  int       m_RCTargetBitrate;
  int       m_RCKeepHierarchicalBit;
  bool      m_RCLCULevelRC;
  bool      m_RCUseLCUSeparateModel;
  int       m_RCInitialQP;
  bool      m_RCForceIntraQP;
#if U0132_TARGET_BITS_SATURATION
  bool      m_RCCpbSaturationEnabled;
  uint32_t      m_RCCpbSize;
  double    m_RCInitialCpbFullness;
#endif
  bool      m_TransquantBypassEnabledFlag;                    ///< transquant_bypass_enabled_flag setting in PPS.
  bool      m_CUTransquantBypassFlagForce;                    ///< if transquant_bypass_enabled_flag, then, if true, all CU transquant bypass flags will be set to true.

  CostMode  m_costMode;                                       ///< The cost function to use, primarily when considering lossless coding.

  VPS       m_cVPS;
  DPS       m_dps;
  bool      m_decodingParameterSetEnabled;                   ///< enable decoding parameter set
  bool      m_recalculateQPAccordingToLambda;                 ///< recalculate QP value according to the lambda value
#if HEVC_SEI
  int       m_activeParameterSetsSEIEnabled;                  ///< enable active parameter set SEI message
#endif
#if FIX_HRD_O0189
  bool      m_hrdParametersPresentFlag;                       ///< enable generation of HRD parameters
#endif
  bool      m_vuiParametersPresentFlag;                       ///< enable generation of VUI parameters
  bool      m_aspectRatioInfoPresentFlag;                     ///< Signals whether aspect_ratio_idc is present
#if HEVC_SEI
  bool      m_chromaResamplingFilterHintEnabled;              ///< Signals whether chroma sampling filter hint data is present
  int       m_chromaResamplingHorFilterIdc;                   ///< Specifies the Index of filter to use
  int       m_chromaResamplingVerFilterIdc;                   ///< Specifies the Index of filter to use
#endif
  int       m_aspectRatioIdc;                                 ///< aspect_ratio_idc
  int       m_sarWidth;                                       ///< horizontal size of the sample aspect ratio
  int       m_sarHeight;                                      ///< vertical size of the sample aspect ratio
  bool      m_colourDescriptionPresentFlag;                   ///< Signals whether colour_primaries, transfer_characteristics and matrix_coefficients are present
  int       m_colourPrimaries;                                ///< Indicates chromaticity coordinates of the source primaries
  int       m_transferCharacteristics;                        ///< Indicates the opto-electronic transfer characteristics of the source
  int       m_matrixCoefficients;                             ///< Describes the matrix coefficients used in deriving luma and chroma from RGB primaries
  bool      m_chromaLocInfoPresentFlag;                       ///< Signals whether chroma_sample_loc_type_top_field and chroma_sample_loc_type_bottom_field are present
  int       m_chromaSampleLocTypeTopField;                    ///< Specifies the location of chroma samples for top field
  int       m_chromaSampleLocTypeBottomField;                 ///< Specifies the location of chroma samples for bottom field
  int       m_chromaSampleLocType;                            ///< Specifies the location of chroma samples for progressive content
  bool      m_overscanInfoPresentFlag;                        ///< Signals whether overscan_appropriate_flag is present
  bool      m_overscanAppropriateFlag;                        ///< Indicates whether conformant decoded pictures are suitable for display using overscan
  bool      m_videoSignalTypePresentFlag;                     ///< Signals whether video_format, video_full_range_flag, and colour_description_present_flag are present
  bool      m_videoFullRangeFlag;                             ///< Indicates the black level and range of luma and chroma signals

  bool      m_bEfficientFieldIRAPEnabled;                     ///< enable to code fields in a specific, potentially more efficient, order.
  bool      m_bHarmonizeGopFirstFieldCoupleEnabled;

  std::string m_summaryOutFilename;                           ///< filename to use for producing summary output file.
  std::string m_summaryPicFilenameBase;                       ///< Base filename to use for producing summary picture output files. The actual filenames used will have I.txt, P.txt and B.txt appended.
  uint32_t        m_summaryVerboseness;                           ///< Specifies the level of the verboseness of the text output.
  int       m_ImvMode;
  int       m_Imv4PelFast;
  std::string m_decodeBitstreams[2];                          ///< filename for decode bitstreams.
  bool        m_forceDecodeBitstream1;                        ///< guess what it means
  int         m_switchPOC;                                    ///< dbg poc.
  int         m_switchDQP;                                    ///< dqp applied to  switchPOC and subsequent pictures.
  int         m_fastForwardToPOC;                             ///<
  bool        m_stopAfterFFtoPOC;                             ///<
  int         m_debugCTU;                                     ///< dbg ctu
  bool        m_bs2ModPOCAndType;



#if ENABLE_SPLIT_PARALLELISM
  int         m_numSplitThreads;
  bool        m_forceSingleSplitThread;
#endif
#if ENABLE_WPP_PARALLELISM
  int         m_numWppThreads;
  int         m_numWppExtraLines;
  bool        m_ensureWppBitEqual;
#endif

  bool        m_alf;                                          ///< Adaptive Loop Filter
#if JVET_O0756_CALCULATE_HDRMETRICS
  double                       m_whitePointDeltaE[hdrtoolslib::NB_REF_WHITE];
  double                       m_maxSampleValue;
  hdrtoolslib::SampleRange     m_sampleRange;
  hdrtoolslib::ColorPrimaries  m_colorPrimaries;
  bool                         m_enableTFunctionLUT;
  hdrtoolslib::ChromaLocation  m_chromaLocation[2];
  int                          m_chromaUPFilter;
  int                          m_cropOffsetLeft;
  int                          m_cropOffsetTop;
  int                          m_cropOffsetRight;
  int                          m_cropOffsetBottom;
  bool                         m_calculateHdrMetrics;
#endif  
#if JVET_O1164_RPR
  double      m_scalingRatioHor;
  double      m_scalingRatioVer;
  bool        m_rprEnabled;
  int         m_switchPocPeriod;
  int         m_upscaledOutput;
#endif

public:
  EncCfg()
  : m_tileColumnWidth()
  , m_tileRowHeight()
  {
#if !JVET_O0525_REMOVE_PCM
    m_PCMBitDepth[CHANNEL_TYPE_LUMA]=8;
    m_PCMBitDepth[CHANNEL_TYPE_CHROMA]=8;
#endif
  }

  virtual ~EncCfg()
  {}

  void setProfile(Profile::Name profile) { m_profile = profile; }
  void setLevel(Level::Tier tier, Level::Name level) { m_levelTier = tier; m_level = level; }
#if JVET_O0044_MULTI_SUB_PROFILE
  void setNumSubProfile( uint8_t numSubProfile) { m_numSubProfile = numSubProfile; m_subProfile.resize(m_numSubProfile); }
  void setSubProfile( int i, uint32_t subProfile) { m_subProfile[i] = subProfile; }
#else
  void setSubProfile(uint32_t subProfile) { m_subProfile = subProfile; }
#endif
  bool      getIntraOnlyConstraintFlag() const { return m_bIntraOnlyConstraintFlag; }
  void      setIntraOnlyConstraintFlag(bool bVal) { m_bIntraOnlyConstraintFlag = bVal; }
  uint32_t  getMaxBitDepthConstraintIdc() const { return m_maxBitDepthConstraintIdc; }
  void      setMaxBitDepthConstraintIdc(uint32_t u) { m_maxBitDepthConstraintIdc = u; }
  uint32_t  getMaxChromaFormatConstraintIdc() const { return m_maxChromaFormatConstraintIdc; }
  void      setMaxChromaFormatConstraintIdc(uint32_t u) { m_maxChromaFormatConstraintIdc = u; }
  bool      getFrameConstraintFlag() const { return m_bFrameConstraintFlag; }
  void      setFrameConstraintFlag(bool bVal) { m_bFrameConstraintFlag = bVal; }
  bool      getNoQtbttDualTreeIntraConstraintFlag() const { return m_bNoQtbttDualTreeIntraConstraintFlag; }
  void      setNoQtbttDualTreeIntraConstraintFlag(bool bVal) { m_bNoQtbttDualTreeIntraConstraintFlag = bVal; }
  bool      getNoPartitionConstraintsOverrideConstraintFlag() const { return m_noPartitionConstraintsOverrideConstraintFlag; }
  void      setNoPartitionConstraintsOverrideConstraintFlag(bool bVal) { m_noPartitionConstraintsOverrideConstraintFlag = bVal; }
  bool      getNoSaoConstraintFlag() const { return m_bNoSaoConstraintFlag; }
  void      setNoSaoConstraintFlag(bool bVal) { m_bNoSaoConstraintFlag = bVal; }
  bool      getNoAlfConstraintFlag() const { return m_bNoAlfConstraintFlag; }
  void      setNoAlfConstraintFlag(bool bVal) { m_bNoAlfConstraintFlag = bVal; }
#if !JVET_O0525_REMOVE_PCM
  bool      getNoPcmConstraintFlag() const { return m_bNoPcmConstraintFlag; }
  void      setNoPcmConstraintFlag(bool bVal) { m_bNoPcmConstraintFlag = bVal; }
#endif
  bool      getNoRefWraparoundConstraintFlag() const { return m_bNoRefWraparoundConstraintFlag; }
  void      setNoRefWraparoundConstraintFlag(bool bVal) { m_bNoRefWraparoundConstraintFlag = bVal; }
  bool      getNoTemporalMvpConstraintFlag() const { return m_bNoTemporalMvpConstraintFlag; }
  void      setNoTemporalMvpConstraintFlag(bool bVal) { m_bNoTemporalMvpConstraintFlag = bVal; }
  bool      getNoSbtmvpConstraintFlag() const { return m_bNoSbtmvpConstraintFlag; }
  void      setNoSbtmvpConstraintFlag(bool bVal) { m_bNoSbtmvpConstraintFlag = bVal; }
  bool      getNoAmvrConstraintFlag() const { return m_bNoAmvrConstraintFlag; }
  void      setNoAmvrConstraintFlag(bool bVal) { m_bNoAmvrConstraintFlag = bVal; }
  bool      getNoBdofConstraintFlag() const { return m_bNoBdofConstraintFlag; }
  void      setNoBdofConstraintFlag(bool bVal) { m_bNoBdofConstraintFlag = bVal; }
  bool      getNoDmvrConstraintFlag() const { return m_noDmvrConstraintFlag; }
  void      setNoDmvrConstraintFlag(bool bVal) { m_noDmvrConstraintFlag = bVal; }
  bool      getNoCclmConstraintFlag() const { return m_bNoCclmConstraintFlag; }
  void      setNoCclmConstraintFlag(bool bVal) { m_bNoCclmConstraintFlag = bVal; }
  bool      getNoMtsConstraintFlag() const { return m_bNoMtsConstraintFlag; }
  void      setNoMtsConstraintFlag(bool bVal) { m_bNoMtsConstraintFlag = bVal; }
  bool      getNoSbtConstraintFlag() const { return m_noSbtConstraintFlag; }
  void      setNoSbtConstraintFlag(bool bVal) { m_noSbtConstraintFlag = bVal; }
  bool      getNoAffineMotionConstraintFlag() const { return m_bNoAffineMotionConstraintFlag; }
  void      setNoAffineMotionConstraintFlag(bool bVal) { m_bNoAffineMotionConstraintFlag = bVal; }
  bool      getNoGbiConstraintFlag() const { return m_bNoGbiConstraintFlag; }
  void      setNoGbiConstraintFlag(bool bVal) { m_bNoGbiConstraintFlag = bVal; }
  bool      getNoIbcConstraintFlag() const { return m_noIbcConstraintFlag; }
  void      setNoIbcConstraintFlag(bool bVal) { m_noIbcConstraintFlag = bVal; }
  bool      getNoMhIntraConstraintFlag() const { return m_bNoMhIntraConstraintFlag; }
  void      setNoMhIntraConstraintFlag(bool bVal) { m_bNoMhIntraConstraintFlag = bVal; }
  bool      getNoFPelMmvdConstraintFlag() const { return m_noFPelMmvdConstraintFlag; }
  void      setNoFPelMmvdConstraintFlag(bool bVal) { m_noFPelMmvdConstraintFlag = bVal; }
  bool      getNoTriangleConstraintFlag() const { return m_bNoTriangleConstraintFlag; }
  void      setNoTriangleConstraintFlag(bool bVal) { m_bNoTriangleConstraintFlag = bVal; }
  bool      getNoLadfConstraintFlag() const { return m_bNoLadfConstraintFlag; }
  void      setNoLadfConstraintFlag(bool bVal) { m_bNoLadfConstraintFlag = bVal; }
  bool      getNoTransformSkipConstraintFlag() const { return m_noTransformSkipConstraintFlag; }
  void      setNoTransformSkipConstraintFlag(bool bVal) { m_noTransformSkipConstraintFlag = bVal; }
#if JVET_O1136_TS_BDPCM_SIGNALLING
  bool      getNoBDPCMConstraintFlag() const { return m_noBDPCMConstraintFlag; }
  void      setNoBDPCMConstraintFlag(bool bVal) { m_noBDPCMConstraintFlag = bVal; }
#endif
#if JVET_O0376_SPS_JOINTCBCR_FLAG
  bool      getNoJointCbCrConstraintFlag() const { return m_noJointCbCrConstraintFlag; }
  void      setNoJointCbCrConstraintFlag(bool bVal) { m_noJointCbCrConstraintFlag = bVal; }
#endif
  bool      getNoQpDeltaConstraintFlag() const { return m_bNoQpDeltaConstraintFlag; }
  void      setNoQpDeltaConstraintFlag(bool bVal) { m_bNoQpDeltaConstraintFlag = bVal; }
  bool      getNoDepQuantConstraintFlag() const { return m_bNoDepQuantConstraintFlag; }
  void      setNoDepQuantConstraintFlag(bool bVal) { m_bNoDepQuantConstraintFlag = bVal; }
  bool      getNoSignDataHidingConstraintFlag() const { return m_bNoSignDataHidingConstraintFlag; }
  void      setNoSignDataHidingConstraintFlag(bool bVal) { m_bNoSignDataHidingConstraintFlag = bVal; }

  void      setFrameRate                    ( int   i )      { m_iFrameRate = i; }
  void      setFrameSkip                    ( uint32_t  i )      { m_FrameSkip = i; }
  void      setTemporalSubsampleRatio       ( uint32_t  i )      { m_temporalSubsampleRatio = i; }
  void      setSourceWidth                  ( int   i )      { m_iSourceWidth = i; }
  void      setSourceHeight                 ( int   i )      { m_iSourceHeight = i; }

  Window   &getConformanceWindow()                           { return m_conformanceWindow; }
  void      setConformanceWindow (int confLeft, int confRight, int confTop, int confBottom ) { m_conformanceWindow.setWindow (confLeft, confRight, confTop, confBottom); }

  void      setFramesToBeEncoded            ( int   i )      { m_framesToBeEncoded = i; }

  bool      getPrintMSEBasedSequencePSNR    ()         const { return m_printMSEBasedSequencePSNR;  }
  void      setPrintMSEBasedSequencePSNR    (bool value)     { m_printMSEBasedSequencePSNR = value; }

  bool      getPrintHexPsnr                 ()         const { return m_printHexPsnr;               }
  void      setPrintHexPsnr                 (bool value)     { m_printHexPsnr = value;              }

  bool      getPrintFrameMSE                ()         const { return m_printFrameMSE;              }
  void      setPrintFrameMSE                (bool value)     { m_printFrameMSE = value;             }

  bool      getPrintSequenceMSE             ()         const { return m_printSequenceMSE;           }
  void      setPrintSequenceMSE             (bool value)     { m_printSequenceMSE = value;          }

  bool      getCabacZeroWordPaddingEnabled()           const { return m_cabacZeroWordPaddingEnabled;  }
  void      setCabacZeroWordPaddingEnabled(bool value)       { m_cabacZeroWordPaddingEnabled = value; }

  //====== Coding Structure ========
  void      setIntraPeriod                  (int   i)        { m_uiIntraPeriod = i;                   }
  void      setDecodingRefreshType          ( int   i )      { m_uiDecodingRefreshType = (uint32_t)i; }
  void      setReWriteParamSets             ( bool  b )      { m_rewriteParamSets = b; }
  void      setIDRRefParamListPresent       ( bool  b )      { m_idrRefParamList  = b; }
  bool      getIDRRefParamListPresent       ()        const  { return m_idrRefParamList; }
  void      setGOPSize                      ( int   i )      { m_iGOPSize = i; }
  void      setGopList(const GOPEntry GOPList[MAX_GOP]) { for (int i = 0; i < MAX_GOP; i++) m_GOPList[i] = GOPList[i]; }
  const GOPEntry &getGOPEntry               ( int   i ) const { return m_GOPList[i]; }
  void      setRPLList0(const RPLEntry RPLList[MAX_GOP])
  {
    m_numRPLList0 = 0;
    for (int i = 0; i < MAX_GOP; i++)
    {
      m_RPLList0[i] = RPLList[i];
      if (m_RPLList0[i].m_POC != -1) m_numRPLList0++;
    }
  }
  void      setRPLList1(const RPLEntry RPLList[MAX_GOP])
  {
    m_numRPLList1 = 0;
    for (int i = 0; i < MAX_GOP; i++)
    {
      m_RPLList1[i] = RPLList[i];
      if (m_RPLList1[i].m_POC != -1) m_numRPLList1++;
    }
  }
  const RPLEntry &getRPLEntry(int L01, int idx) const { return (L01 == 0) ? m_RPLList0[idx] : m_RPLList1[idx]; }
  int       getRPLCandidateSize(int L01) const { return  (L01 == 0) ? m_numRPLList0 : m_numRPLList1; }
  void      setEncodedFlag(uint32_t  i, bool value) { m_RPLList0[i].m_isEncoded = value; m_RPLList1[i].m_isEncoded = value; }
  void      setMaxDecPicBuffering           ( uint32_t u, uint32_t tlayer ) { m_maxDecPicBuffering[tlayer] = u;    }
  void      setNumReorderPics               ( int  i, uint32_t tlayer ) { m_numReorderPics[tlayer] = i;    }
#if JVET_N0494_DRAP
  void      setDrapPeriod                   (int drapPeriod) { m_drapPeriod = drapPeriod; }
#endif

  void      setBaseQP                       ( int   i )      { m_iQP = i; }
#if X0038_LAMBDA_FROM_QP_CAPABILITY
  void      setIntraQPOffset                ( int   i )         { m_intraQPOffset = i; }
  void      setLambdaFromQPEnable           ( bool  b )         { m_lambdaFromQPEnable = b; }
#endif
#if JVET_O0650_SIGNAL_CHROMAQP_MAPPING_TABLE
  void      setChromaQpMappingTableParams   (const ChromaQpMappingTableParams &params) { m_chromaQpMappingTableParams = params; }
#endif

  void      setPad                          ( int*  iPad                   )      { for ( int i = 0; i < 2; i++ ) m_aiPad[i] = iPad[i]; }

  int       getMaxRefPicNum                 ()                              { return m_iMaxRefPicNum;           }
  void      setMaxRefPicNum                 ( int iMaxRefPicNum )           { m_iMaxRefPicNum = iMaxRefPicNum;  }

  int       getMaxTempLayer                 ()                              { return m_maxTempLayer;              }
  void      setMaxTempLayer                 ( int maxTempLayer )            { m_maxTempLayer = maxTempLayer;      }

  void      setCTUSize                      ( unsigned  u )      { m_CTUSize  = u; }
  void      setMinQTSizes                   ( unsigned* minQT)   { m_uiMinQT[0] = minQT[0]; m_uiMinQT[1] = minQT[1]; m_uiMinQT[2] = minQT[2]; }
  void      setMaxMTTHierarchyDepth         ( unsigned uiMaxMTTHierarchyDepth, unsigned uiMaxMTTHierarchyDepthI, unsigned uiMaxMTTHierarchyDepthIChroma )
                                                             { m_uiMaxMTTHierarchyDepth = uiMaxMTTHierarchyDepth; m_uiMaxMTTHierarchyDepthI = uiMaxMTTHierarchyDepthI; m_uiMaxMTTHierarchyDepthIChroma = uiMaxMTTHierarchyDepthIChroma; }
  unsigned  getMaxMTTHierarchyDepth         ()         const { return m_uiMaxMTTHierarchyDepth; }
  unsigned  getMaxMTTHierarchyDepthI        ()         const { return m_uiMaxMTTHierarchyDepthI; }
  unsigned  getMaxMTTHierarchyDepthIChroma  ()         const { return m_uiMaxMTTHierarchyDepthIChroma; }
  int       getCTUSize                      ()         const { return m_CTUSize; }
  void      setUseSplitConsOverride         (bool  n)        { m_useSplitConsOverride = n; }
  bool      getUseSplitConsOverride         ()         const { return m_useSplitConsOverride; }
  void      setDualITree                    ( bool b )       { m_dualITree = b; }
  bool      getDualITree                    ()         const { return m_dualITree; }

  void      setLFNST                        ( bool b )       { m_LFNST = b; }
  bool      getLFNST()                                 const { return m_LFNST; }
  void      setUseFastLFNST                 ( bool b )       { m_useFastLFNST = b; }
  bool      getUseFastLFNST()                          const { return m_useFastLFNST; }

  void      setUseLMChroma                  ( int n )        { m_LMChroma = n; }
  int       getUseLMChroma()                           const { return m_LMChroma; }
  void      setCclmCollocatedChromaFlag     ( bool b )       { m_cclmCollocatedChromaFlag = b; }
  bool      getCclmCollocatedChromaFlag     ()         const { return m_cclmCollocatedChromaFlag; }

  void      setSubPuMvpMode(int n)          { m_SubPuMvpMode = n; }
  bool      getSubPuMvpMode()         const { return m_SubPuMvpMode; }

  void      setAffine                       ( bool b )       { m_Affine = b; }
  bool      getAffine                       ()         const { return m_Affine; }
  void      setAffineType( bool b )                          { m_AffineType = b; }
  bool      getAffineType()                            const { return m_AffineType; }
#if JVET_O0070_PROF
  void      setPROF                         (bool b)         { m_PROF = b; }
  bool      getPROF                         ()         const { return m_PROF; }
#endif
  void      setBIO(bool b)                                   { m_BIO = b; }
  bool      getBIO()                                   const { return m_BIO; }

  void      setIntraMTSMaxCand              ( unsigned u )   { m_IntraMTSMaxCand = u; }
  unsigned  getIntraMTSMaxCand              ()         const { return m_IntraMTSMaxCand; }
  void      setInterMTSMaxCand              ( unsigned u )   { m_InterMTSMaxCand = u; }
  unsigned  getInterMTSMaxCand              ()         const { return m_InterMTSMaxCand; }
  void      setIntraMTS                     ( bool b )       { m_IntraMTS = b; }
  bool      getIntraMTS                     ()         const { return m_IntraMTS; }
  void      setInterMTS                     ( bool b )       { m_InterMTS = b; }
  bool      getInterMTS                     ()         const { return m_InterMTS; }
  void      setImplicitMTS                  ( bool b )       { m_ImplicitMTS = b; }
  bool      getImplicitMTS                  ()         const { return m_ImplicitMTS; }
  void      setUseSBT                       ( bool b )       { m_SBT = b; }
  bool      getUseSBT                       ()         const { return m_SBT; }

  void      setUseCompositeRef              (bool b)         { m_compositeRefEnabled = b; }
  bool      getUseCompositeRef              ()         const { return m_compositeRefEnabled; }
  void      setUseSMVD                      ( bool b )       { m_SMVD = b; }
  bool      getUseSMVD                      ()         const { return m_SMVD; }
  void      setUseGBi                       ( bool b )       { m_GBi = b; }
  bool      getUseGBi                       ()         const { return m_GBi; }
  void      setUseGBiFast                   ( uint32_t b )   { m_GBiFast = b; }
  bool      getUseGBiFast                   ()         const { return m_GBiFast; }

#if LUMA_ADAPTIVE_DEBLOCKING_FILTER_QP_OFFSET
  void      setUseLadf                      ( bool b )       { m_LadfEnabled = b; }
  bool      getUseLadf                      ()         const { return m_LadfEnabled; }
  void      setLadfNumIntervals             ( int i )        { m_LadfNumIntervals = i; }
  int       getLadfNumIntervals             ()         const { return m_LadfNumIntervals; }
  void      setLadfQpOffset                 ( int value, int idx ){ m_LadfQpOffset[ idx ] = value; }
  int       getLadfQpOffset                 ( int idx ) const { return m_LadfQpOffset[ idx ]; }
  void      setLadfIntervalLowerBound       ( int value, int idx ){ m_LadfIntervalLowerBound[ idx ] = value; }
  int       getLadfIntervalLowerBound       ( int idx ) const { return m_LadfIntervalLowerBound[ idx ]; }

#endif

  void      setUseMHIntra                   ( bool b )       { m_MHIntra = b; }
  bool      getUseMHIntra                   ()         const { return m_MHIntra; }
  void      setUseTriangle                  ( bool b )       { m_Triangle = b; }
  bool      getUseTriangle                  ()         const { return m_Triangle; }
  void      setAllowDisFracMMVD             ( bool b )       { m_allowDisFracMMVD = b;    }
  bool      getAllowDisFracMMVD             ()         const { return m_allowDisFracMMVD; }
  void      setUseHashME                    ( bool b )       { m_HashME = b; }
  bool      getUseHashME                    ()         const { return m_HashME; }
  void      setUseAffineAmvr                ( bool b )       { m_AffineAmvr = b;    }
  bool      getUseAffineAmvr                ()         const { return m_AffineAmvr; }
  void      setUseAffineAmvrEncOpt          ( bool b )       { m_AffineAmvrEncOpt = b;    }
  bool      getUseAffineAmvrEncOpt          ()         const { return m_AffineAmvrEncOpt; }
  void      setDMVR                      ( bool b )       { m_DMVR = b; }
  bool      getDMVR                      ()         const { return m_DMVR; }
  void      setMMVD                         (bool b)         { m_MMVD = b;    }
  bool      getMMVD                         ()         const { return m_MMVD; }
  void      setMmvdDisNum                   ( int b )        { m_MmvdDisNum = b; }
  int       getMmvdDisNum                   ()         const { return m_MmvdDisNum; }
 #if !JVET_O1136_TS_BDPCM_SIGNALLING 
  void      setRDPCM                     ( bool b )       { m_RdpcmMode = b; }
  bool      getRDPCM                     ()         const { return m_RdpcmMode; }
 #endif 
#if JVET_O0119_BASE_PALETTE_444
  void      setPLTMode                   ( unsigned n)    { m_PLTMode = n; }
  unsigned  getPLTMode                   ()         const { return m_PLTMode; }
#endif
#if JVET_O0376_SPS_JOINTCBCR_FLAG
  void      setJointCbCr                    ( bool b )       { m_JointCbCrMode = b; }
  bool      getJointCbCr                    ()         const { return m_JointCbCrMode; }
#endif
  void      setIBCMode                      (unsigned n)     { m_IBCMode = n; }
  unsigned  getIBCMode                      ()         const { return m_IBCMode; }
  void      setIBCLocalSearchRangeX         (unsigned n)     { m_IBCLocalSearchRangeX = n; }
  unsigned  getIBCLocalSearchRangeX         ()         const { return m_IBCLocalSearchRangeX; }
  void      setIBCLocalSearchRangeY         (unsigned n)     { m_IBCLocalSearchRangeY = n; }
  unsigned  getIBCLocalSearchRangeY         ()         const { return m_IBCLocalSearchRangeY; }
  void      setIBCHashSearch                (unsigned n)     { m_IBCHashSearch = n; }
  unsigned  getIBCHashSearch                ()         const { return m_IBCHashSearch; }
  void      setIBCHashSearchMaxCand         (unsigned n)     { m_IBCHashSearchMaxCand = n; }
  unsigned  getIBCHashSearchMaxCand         ()         const { return m_IBCHashSearchMaxCand; }
  void      setIBCHashSearchRange4SmallBlk  (unsigned n)     { m_IBCHashSearchRange4SmallBlk = n; }
  unsigned  getIBCHashSearchRange4SmallBlk  ()         const { return m_IBCHashSearchRange4SmallBlk; }
  void      setIBCFastMethod                (unsigned n)     { m_IBCFastMethod = n; }
  unsigned  getIBCFastMethod                ()         const { return m_IBCFastMethod; }

  void      setUseWrapAround                ( bool b )       { m_wrapAround = b; }
  bool      getUseWrapAround                ()         const { return m_wrapAround; }
  void      setWrapAroundOffset             ( unsigned u )   { m_wrapAroundOffset = u; }
  unsigned  getWrapAroundOffset             ()         const { return m_wrapAroundOffset; }

  // ADD_NEW_TOOL : (encoder lib) add access functions here
  void      setLoopFilterAcrossVirtualBoundariesDisabledFlag( bool b ) { m_loopFilterAcrossVirtualBoundariesDisabledFlag = b; }
  bool      getLoopFilterAcrossVirtualBoundariesDisabledFlag() const { return m_loopFilterAcrossVirtualBoundariesDisabledFlag; }
  void      setNumVerVirtualBoundaries      ( unsigned u )   { m_numVerVirtualBoundaries = u; }
  unsigned  getNumVerVirtualBoundaries      ()         const { return m_numVerVirtualBoundaries; }
  void      setNumHorVirtualBoundaries      ( unsigned u )   { m_numHorVirtualBoundaries = u; }
  unsigned  getNumHorVirtualBoundaries      ()         const { return m_numHorVirtualBoundaries; }
  void      setVirtualBoundariesPosX        ( unsigned u, unsigned idx ) { m_virtualBoundariesPosX[idx] = u; }
  unsigned  getVirtualBoundariesPosX        ( unsigned idx ) const { return m_virtualBoundariesPosX[idx]; }
  void      setVirtualBoundariesPosY        ( unsigned u, unsigned idx ) { m_virtualBoundariesPosY[idx] = u; }
  unsigned  getVirtualBoundariesPosY        ( unsigned idx ) const { return m_virtualBoundariesPosY[idx]; }
  void      setUseISP                       ( bool b )       { m_ISP = b; }
  bool      getUseISP                       ()         const { return m_ISP; }
  void      setReshaper                     ( bool b )                   { m_lumaReshapeEnable = b; }
  bool      getReshaper                     () const                     { return m_lumaReshapeEnable; }
  void      setReshapeSignalType            ( uint32_t signalType )      { m_reshapeSignalType = signalType; }
  uint32_t  getReshapeSignalType            () const                     { return m_reshapeSignalType; }
  void      setReshapeIntraCMD              (uint32_t intraCMD)          { m_intraCMD = intraCMD; }
  uint32_t  getReshapeIntraCMD              ()                           { return m_intraCMD; }
  void      setReshapeCW                    (const ReshapeCW &reshapeCW) { m_reshapeCW = reshapeCW; }
  const ReshapeCW& getReshapeCW             ()                           { return m_reshapeCW; }
  void      setMaxCUWidth                   ( uint32_t  u )      { m_maxCUWidth  = u; }
  uint32_t      getMaxCUWidth                   () const         { return m_maxCUWidth; }
  void      setMaxCUHeight                  ( uint32_t  u )      { m_maxCUHeight = u; }
  uint32_t      getMaxCUHeight                  () const         { return m_maxCUHeight; }
  void      setMaxCodingDepth               ( uint32_t  u )      { m_maxTotalCUDepth = u; }
  uint32_t      getMaxCodingDepth               () const         { return m_maxTotalCUDepth; }
  void      setLog2DiffMaxMinCodingBlockSize( uint32_t  u )      { m_log2DiffMaxMinCodingBlockSize = u; }
  void      setUseEncDbOpt                  ( bool  n )          { m_encDbOpt = n; }
  bool      getUseEncDbOpt                  () const             { return m_encDbOpt; }

  void      setUseFastLCTU                  ( bool  n )      { m_useFastLCTU = n; }
  bool      getUseFastLCTU                  () const         { return m_useFastLCTU; }
  void      setUseFastMerge                 ( bool  n )      { m_useFastMrg = n; }
  bool      getUseFastMerge                 () const         { return m_useFastMrg; }
  void      setUsePbIntraFast               ( bool  n )      { m_usePbIntraFast = n; }
  bool      getUsePbIntraFast               () const         { return m_usePbIntraFast; }
  void      setUseAMaxBT                    ( bool  n )      { m_useAMaxBT = n; }
  bool      getUseAMaxBT                    () const         { return m_useAMaxBT; }

  void      setUseE0023FastEnc              ( bool b )       { m_e0023FastEnc = b; }
  bool      getUseE0023FastEnc              () const         { return m_e0023FastEnc; }
  void      setUseContentBasedFastQtbt      ( bool b )       { m_contentBasedFastQtbt = b; }
  bool      getUseContentBasedFastQtbt      () const         { return m_contentBasedFastQtbt; }
  void      setUseNonLinearAlfLuma          ( bool b )       { m_useNonLinearAlfLuma = b; }
  bool      getUseNonLinearAlfLuma          () const         { return m_useNonLinearAlfLuma; }
  void      setUseNonLinearAlfChroma        ( bool b )       { m_useNonLinearAlfChroma = b; }
  bool      getUseNonLinearAlfChroma        () const         { return m_useNonLinearAlfChroma; }
#if JVET_O0090_ALF_CHROMA_FILTER_ALTERNATIVES_CTB
  void      setMaxNumAlfAlternativesChroma  ( uint32_t u )   { m_maxNumAlfAlternativesChroma = u; }
  uint32_t  getMaxNumAlfAlternativesChroma  () const         { return m_maxNumAlfAlternativesChroma; }
#endif
  void      setUseMIP                       ( bool b )       { m_MIP = b; }
  bool      getUseMIP                       () const         { return m_MIP; }
  void      setUseFastMIP                   ( bool b )       { m_useFastMIP = b; }
  bool      getUseFastMIP                   () const         { return m_useFastMIP; }
#if JVET_O0050_LOCAL_DUAL_TREE
  void     setFastLocalDualTreeMode         ( int i )        { m_fastLocalDualTreeMode = i; }
  int      getFastLocalDualTreeMode         () const         { return m_fastLocalDualTreeMode; }
#endif

#if MAX_TB_SIZE_SIGNALLING
  void      setLog2MaxTbSize                ( uint32_t  u )   { m_log2MaxTbSize = u; }
#endif

  //====== Loop/Deblock Filter ========
  void      setLoopFilterDisable            ( bool  b )      { m_bLoopFilterDisable       = b; }
  void      setLoopFilterOffsetInPPS        ( bool  b )      { m_loopFilterOffsetInPPS      = b; }
  void      setLoopFilterBetaOffset         ( int   i )      { m_loopFilterBetaOffsetDiv2  = i; }
  void      setLoopFilterTcOffset           ( int   i )      { m_loopFilterTcOffsetDiv2    = i; }
#if W0038_DB_OPT
  void      setDeblockingFilterMetric       ( int   i )      { m_deblockingFilterMetric = i; }
#else
  void      setDeblockingFilterMetric       ( bool  b )      { m_DeblockingFilterMetric = b; }
#endif
  //====== Motion search ========
  void      setDisableIntraPUsInInterSlices ( bool  b )      { m_bDisableIntraPUsInInterSlices = b; }
  void      setMotionEstimationSearchMethod ( MESearchMethod e ) { m_motionEstimationSearchMethod = e; }
  void      setSearchRange                  ( int   i )      { m_iSearchRange = i; }
  void      setBipredSearchRange            ( int   i )      { m_bipredSearchRange = i; }
  void      setClipForBiPredMeEnabled       ( bool  b )      { m_bClipForBiPredMeEnabled = b; }
  void      setFastMEAssumingSmootherMVEnabled ( bool b )    { m_bFastMEAssumingSmootherMVEnabled = b; }
  void      setMinSearchWindow              ( int   i )      { m_minSearchWindow = i; }
  void      setRestrictMESampling           ( bool  b )      { m_bRestrictMESampling = b; }

  //====== Quality control ========
  void      setMaxDeltaQP                   ( int   i )      { m_iMaxDeltaQP = i; }
  void      setCuQpDeltaSubdiv              ( int   i )      { m_cuQpDeltaSubdiv = i; }
  int       getCuChromaQpOffsetSubdiv       ()         const { return m_cuChromaQpOffsetSubdiv;  }
  void      setCuChromaQpOffsetSubdiv       (int value)      { m_cuChromaQpOffsetSubdiv = value; }

  void      setChromaCbQpOffset             ( int   i )      { m_chromaCbQpOffset = i; }
  void      setChromaCrQpOffset             ( int   i )      { m_chromaCrQpOffset = i; }
  void      setChromaCbQpOffsetDualTree     ( int   i )      { m_chromaCbQpOffsetDualTree = i; }
  void      setChromaCrQpOffsetDualTree     ( int   i )      { m_chromaCrQpOffsetDualTree = i; }
  int       getChromaCbQpOffsetDualTree     ()         const { return m_chromaCbQpOffsetDualTree; }
  int       getChromaCrQpOffsetDualTree     ()         const { return m_chromaCrQpOffsetDualTree; }
  void      setChromaCbCrQpOffset           ( int   i )      { m_chromaCbCrQpOffset = i; }
  void      setChromaCbCrQpOffsetDualTree   ( int   i )      { m_chromaCbCrQpOffsetDualTree = i; }
  int       getChromaCbCrQpOffsetDualTree   ()         const { return m_chromaCbCrQpOffsetDualTree; }
#if ER_CHROMA_QP_WCG_PPS
  void      setWCGChromaQpControl           ( const WCGChromaQPControl &ctrl )     { m_wcgChromaQpControl = ctrl; }
  const WCGChromaQPControl &getWCGChromaQPControl () const { return m_wcgChromaQpControl; }
#endif
#if W0038_CQP_ADJ
  void      setSliceChromaOffsetQpIntraOrPeriodic( uint32_t periodicity, int sliceChromaQpOffsetIntraOrPeriodic[2]) { m_sliceChromaQpOffsetPeriodicity = periodicity; memcpy(m_sliceChromaQpOffsetIntraOrPeriodic, sliceChromaQpOffsetIntraOrPeriodic, sizeof(m_sliceChromaQpOffsetIntraOrPeriodic)); }
  int       getSliceChromaOffsetQpIntraOrPeriodic( bool bIsCr) const                                            { return m_sliceChromaQpOffsetIntraOrPeriodic[bIsCr?1:0]; }
  uint32_t      getSliceChromaOffsetQpPeriodicity() const                                                           { return m_sliceChromaQpOffsetPeriodicity; }
#endif

  void      setChromaFormatIdc              ( ChromaFormat cf ) { m_chromaFormatIDC = cf; }
#if REUSE_CU_RESULTS
  ChromaFormat  getChromaFormatIdc          ( ) const        { return m_chromaFormatIDC; }
#else
  ChromaFormat  getChromaFormatIdc          ( )              { return m_chromaFormatIDC; }
#endif

#if SHARP_LUMA_DELTA_QP
  void      setLumaLevelToDeltaQPControls( const LumaLevelToDeltaQPMapping &lumaLevelToDeltaQPMapping ) { m_lumaLevelToDeltaQPMapping=lumaLevelToDeltaQPMapping; }
  const LumaLevelToDeltaQPMapping& getLumaLevelToDeltaQPMapping() const { return m_lumaLevelToDeltaQPMapping; }
#endif

  bool      getExtendedPrecisionProcessingFlag         ()         const { return m_extendedPrecisionProcessingFlag;  }
  void      setExtendedPrecisionProcessingFlag         (bool value)     { m_extendedPrecisionProcessingFlag = value; }

  bool      getHighPrecisionOffsetsEnabledFlag() const { return m_highPrecisionOffsetsEnabledFlag; }
  void      setHighPrecisionOffsetsEnabledFlag(bool value) { m_highPrecisionOffsetsEnabledFlag = value; }

  void      setUseAdaptiveQP                ( bool  b )      { m_bUseAdaptiveQP = b; }
  void      setQPAdaptationRange            ( int   i )      { m_iQPAdaptationRange = i; }
#if ENABLE_QPA
  void      setUsePerceptQPA                ( const bool b ) { m_bUsePerceptQPA = b; }
  void      setUseWPSNR                     ( const bool b ) { m_bUseWPSNR = b; }
#endif

  //====== Sequence ========
  int       getFrameRate                    () const     { return  m_iFrameRate; }
  uint32_t      getFrameSkip                    () const     { return  m_FrameSkip; }
  uint32_t      getTemporalSubsampleRatio       () const     { return  m_temporalSubsampleRatio; }
  int       getSourceWidth                  () const     { return  m_iSourceWidth; }
  int       getSourceHeight                 () const     { return  m_iSourceHeight; }
  int       getFramesToBeEncoded            () const     { return  m_framesToBeEncoded; }

  //====== Lambda Modifiers ========
  void      setLambdaModifier               ( uint32_t uiIndex, double dValue ) { m_adLambdaModifier[ uiIndex ] = dValue; }
  double    getLambdaModifier               ( uint32_t uiIndex )          const { return m_adLambdaModifier[ uiIndex ]; }
  void      setIntraLambdaModifier          ( const std::vector<double> &dValue )               { m_adIntraLambdaModifier = dValue;       }
  const std::vector<double>& getIntraLambdaModifier()                        const { return m_adIntraLambdaModifier;         }
  void      setIntraQpFactor                ( double dValue )               { m_dIntraQpFactor = dValue;              }
  double    getIntraQpFactor                ()                        const { return m_dIntraQpFactor;                }

  //==== Coding Structure ========
  uint32_t      getIntraPeriod                  () const     { return  m_uiIntraPeriod; }
  uint32_t      getDecodingRefreshType          () const     { return  m_uiDecodingRefreshType; }
  bool      getReWriteParamSets             ()  const    { return m_rewriteParamSets; }
  int       getGOPSize                      () const     { return  m_iGOPSize; }
  int       getMaxDecPicBuffering           (uint32_t tlayer) { return m_maxDecPicBuffering[tlayer]; }
  int       getNumReorderPics               (uint32_t tlayer) { return m_numReorderPics[tlayer]; }
#if JVET_N0494_DRAP
  int       getDrapPeriod                   ()     { return m_drapPeriod; }
#endif
#if X0038_LAMBDA_FROM_QP_CAPABILITY
  int       getIntraQPOffset                () const    { return  m_intraQPOffset; }
  int       getLambdaFromQPEnable           () const    { return  m_lambdaFromQPEnable; }
public:
  int       getBaseQP                       () const { return  m_iQP; } // public should use getQPForPicture.
  int       getQPForPicture                 (const uint32_t gopIndex, const Slice *pSlice) const; // Function actually defined in EncLib.cpp
#else
  int       getBaseQP                       ()       { return  m_iQP; }
#endif
  int       getPad                          ( int i )      { CHECK(i >= 2, "Invalid index");                      return  m_aiPad[i]; }

  bool      getAccessUnitDelimiter() const  { return m_AccessUnitDelimiter; }
  void      setAccessUnitDelimiter(bool val){ m_AccessUnitDelimiter = val; }

  //==== Loop/Deblock Filter ========
  bool      getLoopFilterDisable            ()      { return  m_bLoopFilterDisable;       }
  bool      getLoopFilterOffsetInPPS        ()      { return m_loopFilterOffsetInPPS; }
  int       getLoopFilterBetaOffset         ()      { return m_loopFilterBetaOffsetDiv2; }
  int       getLoopFilterTcOffset           ()      { return m_loopFilterTcOffsetDiv2; }
#if W0038_DB_OPT
  int       getDeblockingFilterMetric       ()      { return m_deblockingFilterMetric; }
#else
  bool      getDeblockingFilterMetric       ()      { return m_DeblockingFilterMetric; }
#endif

  //==== Motion search ========
  bool      getDisableIntraPUsInInterSlices    () const { return m_bDisableIntraPUsInInterSlices; }
  MESearchMethod getMotionEstimationSearchMethod ( ) const { return m_motionEstimationSearchMethod; }
  int       getSearchRange                     () const { return m_iSearchRange; }
  bool      getClipForBiPredMeEnabled          () const { return m_bClipForBiPredMeEnabled; }
  bool      getFastMEAssumingSmootherMVEnabled () const { return m_bFastMEAssumingSmootherMVEnabled; }
  int       getMinSearchWindow                 () const { return m_minSearchWindow; }
  bool      getRestrictMESampling              () const { return m_bRestrictMESampling; }

  //==== Quality control ========
  int       getMaxDeltaQP                   () const { return m_iMaxDeltaQP; }
  int       getCuQpDeltaSubdiv              () const { return m_cuQpDeltaSubdiv; }
  bool      getUseAdaptiveQP                () const { return m_bUseAdaptiveQP; }
  int       getQPAdaptationRange            () const { return m_iQPAdaptationRange; }
#if ENABLE_QPA
  bool      getUsePerceptQPA                () const { return m_bUsePerceptQPA; }
  bool      getUseWPSNR                     () const { return m_bUseWPSNR; }
#endif

  //==== Tool list ========
  void      setBitDepth( const ChannelType chType, int internalBitDepthForChannel ) { m_bitDepth[chType] = internalBitDepthForChannel; }
  void      setInputBitDepth( const ChannelType chType, int internalBitDepthForChannel ) { m_inputBitDepth[chType] = internalBitDepthForChannel; }
#if JVET_O1164_PS
  int*      getInputBitDepth()                              { return m_inputBitDepth; }
#endif
  void      setUseASR                       ( bool  b )     { m_bUseASR     = b; }
  void      setUseHADME                     ( bool  b )     { m_bUseHADME   = b; }
  void      setUseRDOQ                      ( bool  b )     { m_useRDOQ    = b; }
  void      setUseRDOQTS                    ( bool  b )     { m_useRDOQTS  = b; }
#if T0196_SELECTIVE_RDOQ
  void      setUseSelectiveRDOQ             ( bool b )      { m_useSelectiveRDOQ = b; }
#endif
  void      setRDpenalty                    ( uint32_t  u )     { m_rdPenalty  = u; }
  void      setFastInterSearchMode          ( FastInterSearchMode m ) { m_fastInterSearchMode = m; }
  void      setUseEarlyCU                   ( bool  b )     { m_bUseEarlyCU = b; }
  void      setUseFastDecisionForMerge      ( bool  b )     { m_useFastDecisionForMerge = b; }
  void      setUseCbfFastMode               ( bool  b )     { m_bUseCbfFastMode = b; }
  void      setUseEarlySkipDetection        ( bool  b )     { m_useEarlySkipDetection = b; }
  void      setUseConstrainedIntraPred      ( bool  b )     { m_bUseConstrainedIntraPred = b; }
  void      setFastUDIUseMPMEnabled         ( bool  b )     { m_bFastUDIUseMPMEnabled = b; }
  void      setFastMEForGenBLowDelayEnabled ( bool  b )     { m_bFastMEForGenBLowDelayEnabled = b; }
  void      setUseBLambdaForNonKeyLowDelayPictures ( bool b ) { m_bUseBLambdaForNonKeyLowDelayPictures = b; }

#if !JVET_O0525_REMOVE_PCM
  void      setPCMInputBitDepthFlag         ( bool  b )     { m_bPCMInputBitDepthFlag = b; }
  void      setPCMFilterDisableFlag         ( bool  b )     {  m_bPCMFilterDisableFlag = b; }
  void      setUsePCM                       ( bool  b )     {  m_usePCM = b;               }
  void      setPCMBitDepth( const ChannelType chType, int pcmBitDepthForChannel ) { m_PCMBitDepth[chType] = pcmBitDepthForChannel; }
  void      setPCMLog2MaxSize               ( uint32_t u )      { m_pcmLog2MaxSize = u;      }
  void      setPCMLog2MinSize               ( uint32_t u )     { m_uiPCMLog2MinSize = u;      }
#endif
  void      setdQPs                         ( int*  p )     { m_aidQP       = p; }
  void      setDeltaQpRD                    ( uint32_t  u )     {m_uiDeltaQpRD  = u; }
  void      setFastDeltaQp                  ( bool  b )     {m_bFastDeltaQP = b; }
  int       getBitDepth                     (const ChannelType chType) const { return m_bitDepth[chType]; }
#if JVET_O1164_PS
  int*      getBitDepth                     ()      { return m_bitDepth; }
#endif
  bool      getUseASR                       ()      { return m_bUseASR;     }
  bool      getUseHADME                     ()      { return m_bUseHADME;   }
  bool      getUseRDOQ                      ()      { return m_useRDOQ;    }
  bool      getUseRDOQTS                    ()      { return m_useRDOQTS;  }
#if T0196_SELECTIVE_RDOQ
  bool      getUseSelectiveRDOQ             ()      { return m_useSelectiveRDOQ; }
#endif
  int       getRDpenalty                    ()      { return m_rdPenalty;  }
  FastInterSearchMode getFastInterSearchMode() const{ return m_fastInterSearchMode;  }
  bool      getUseEarlyCU                   () const{ return m_bUseEarlyCU; }
  bool      getUseFastDecisionForMerge      () const{ return m_useFastDecisionForMerge; }
  bool      getUseCbfFastMode               () const{ return m_bUseCbfFastMode; }
  bool      getUseEarlySkipDetection        () const{ return m_useEarlySkipDetection; }
  bool      getUseConstrainedIntraPred      ()      { return m_bUseConstrainedIntraPred; }
  bool      getFastUDIUseMPMEnabled         ()      { return m_bFastUDIUseMPMEnabled; }
  bool      getFastMEForGenBLowDelayEnabled ()      { return m_bFastMEForGenBLowDelayEnabled; }
  bool      getUseBLambdaForNonKeyLowDelayPictures () { return m_bUseBLambdaForNonKeyLowDelayPictures; }
#if !JVET_O0525_REMOVE_PCM
  bool      getPCMInputBitDepthFlag         ()      { return m_bPCMInputBitDepthFlag;   }
  bool      getPCMFilterDisableFlag         ()      { return m_bPCMFilterDisableFlag;   }
  bool      getUsePCM                       ()      { return m_usePCM;                 }
  uint32_t      getPCMLog2MaxSize               ()      { return m_pcmLog2MaxSize;  }
  uint32_t      getPCMLog2MinSize               ()      { return  m_uiPCMLog2MinSize;  }
#endif

  bool      getCrossComponentPredictionEnabledFlag     ()                const { return m_crossComponentPredictionEnabledFlag;   }
  void      setCrossComponentPredictionEnabledFlag     (const bool value)      { m_crossComponentPredictionEnabledFlag = value;  }
  bool      getUseReconBasedCrossCPredictionEstimate ()                const { return m_reconBasedCrossCPredictionEstimate;  }
  void      setUseReconBasedCrossCPredictionEstimate (const bool value)      { m_reconBasedCrossCPredictionEstimate = value; }
  void      setLog2SaoOffsetScale(ChannelType type, uint32_t uiBitShift)         { m_log2SaoOffsetScale[type] = uiBitShift; }

  bool getUseTransformSkip                             ()      { return m_useTransformSkip;        }
  void setUseTransformSkip                             ( bool b ) { m_useTransformSkip  = b;       }
  bool getTransformSkipRotationEnabledFlag             ()            const { return m_transformSkipRotationEnabledFlag;  }
  void setTransformSkipRotationEnabledFlag             (const bool value)  { m_transformSkipRotationEnabledFlag = value; }
  bool getTransformSkipContextEnabledFlag              ()            const { return m_transformSkipContextEnabledFlag;  }
  void setTransformSkipContextEnabledFlag              (const bool value)  { m_transformSkipContextEnabledFlag = value; }
#if JVET_O1136_TS_BDPCM_SIGNALLING
  bool getUseBDPCM                                     ()         { return m_useBDPCM; }
  void setUseBDPCM                                     ( bool b ) { m_useBDPCM  = b;   }
#endif
#if JVET_O0376_SPS_JOINTCBCR_FLAG
  bool getUseJointCbCr                                 ()         { return m_JointCbCrMode; }
  void setUseJointCbCr                                 (bool b)   { m_JointCbCrMode = b; }
#endif
  bool getPersistentRiceAdaptationEnabledFlag          ()                 const { return m_persistentRiceAdaptationEnabledFlag;  }
  void setPersistentRiceAdaptationEnabledFlag          (const bool value)       { m_persistentRiceAdaptationEnabledFlag = value; }
  bool getCabacBypassAlignmentEnabledFlag              ()       const      { return m_cabacBypassAlignmentEnabledFlag;  }
  void setCabacBypassAlignmentEnabledFlag              (const bool value)  { m_cabacBypassAlignmentEnabledFlag = value; }
  bool getRdpcmEnabledFlag                             (const RDPCMSignallingMode signallingMode)        const      { return m_rdpcmEnabledFlag[signallingMode];  }
  void setRdpcmEnabledFlag                             (const RDPCMSignallingMode signallingMode, const bool value) { m_rdpcmEnabledFlag[signallingMode] = value; }
  bool getUseTransformSkipFast                         ()      { return m_useTransformSkipFast;    }
  void setUseTransformSkipFast                         ( bool b ) { m_useTransformSkipFast  = b;   }
  uint32_t getLog2MaxTransformSkipBlockSize                () const      { return m_log2MaxTransformSkipBlockSize;     }
  void setLog2MaxTransformSkipBlockSize                ( uint32_t u )    { m_log2MaxTransformSkipBlockSize  = u;       }
  bool getIntraSmoothingDisabledFlag               ()      const { return m_intraSmoothingDisabledFlag; }
  void setIntraSmoothingDisabledFlag               (bool bValue) { m_intraSmoothingDisabledFlag=bValue; }
  bool getUseFastISP                                   ()         { return m_useFastISP;    }
  void setUseFastISP                                   ( bool b ) { m_useFastISP  = b;   }

  const int* getdQPs                        () const { return m_aidQP;       }
  uint32_t      getDeltaQpRD                    () const { return m_uiDeltaQpRD; }
  bool      getFastDeltaQp                  () const { return m_bFastDeltaQP; }

  //====== Slice ========
  void  setSliceMode                   ( SliceConstraint  i )        { m_sliceMode = i;              }
  void  setSliceArgument               ( int  i )                    { m_sliceArgument = i;          }
  SliceConstraint getSliceMode         () const                      { return m_sliceMode;           }
  int   getSliceArgument               ()                            { return m_sliceArgument;       }
  //====== Dependent Slice ========
  void  setSliceSegmentMode            ( SliceConstraint  i )        { m_sliceSegmentMode = i;       }
  void  setSliceSegmentArgument        ( int  i )                    { m_sliceSegmentArgument = i;   }
  SliceConstraint getSliceSegmentMode  () const                      { return m_sliceSegmentMode;    }
  int   getSliceSegmentArgument        ()                            { return m_sliceSegmentArgument;}
  void      setLFCrossSliceBoundaryFlag     ( bool   bValue  )       { m_bLFCrossSliceBoundaryFlag = bValue; }
  bool      getLFCrossSliceBoundaryFlag     ()                       { return m_bLFCrossSliceBoundaryFlag;   }

  void      setUseSAO                  (bool bVal)                   { m_bUseSAO = bVal; }
  bool      getUseSAO                  ()                            { return m_bUseSAO; }
  void  setTestSAODisableAtPictureLevel (bool bVal)                  { m_bTestSAODisableAtPictureLevel = bVal; }
  bool  getTestSAODisableAtPictureLevel ( ) const                    { return m_bTestSAODisableAtPictureLevel; }

  void   setSaoEncodingRate(double v)                                { m_saoEncodingRate = v; }
  double getSaoEncodingRate() const                                  { return m_saoEncodingRate; }
  void   setSaoEncodingRateChroma(double v)                          { m_saoEncodingRateChroma = v; }
  double getSaoEncodingRateChroma() const                            { return m_saoEncodingRateChroma; }
  void  setMaxNumOffsetsPerPic                   (int iVal)          { m_maxNumOffsetsPerPic = iVal; }
  int   getMaxNumOffsetsPerPic                   ()                  { return m_maxNumOffsetsPerPic; }
  void  setSaoCtuBoundary              (bool val)                    { m_saoCtuBoundary = val; }
  bool  getSaoCtuBoundary              ()                            { return m_saoCtuBoundary; }

  void  setSaoGreedyMergeEnc           (bool val)                    { m_saoGreedyMergeEnc = val; }
  bool  getSaoGreedyMergeEnc           ()                            { return m_saoGreedyMergeEnc; }
  void  setLFCrossTileBoundaryFlag               ( bool   val  )     { m_loopFilterAcrossBricksEnabledFlag = val; }
  bool  getLFCrossTileBoundaryFlag               ()                  { return m_loopFilterAcrossBricksEnabledFlag;   }
  void  setTileUniformSpacingFlag      ( bool b )                    { m_tileUniformSpacingFlag = b; }
  bool  getTileUniformSpacingFlag      ()                            { return m_tileUniformSpacingFlag; }
  void  setNumColumnsMinus1            ( int i )                     { m_iNumColumnsMinus1 = i; }
  int   getNumColumnsMinus1            ()                            { return m_iNumColumnsMinus1; }
#if JVET_O0143_BOTTOM_RIGHT_BRICK_IDX_DELTA
  void  setUniformTileColsWidthMinus1  (int i)                       { m_uniformTileColsWidthMinus1 = i; }
  int   getUniformTileColsWidthMinus1  ()                            { return m_uniformTileColsWidthMinus1; }
  void  setUniformTileRowHeightMinus1  (int i)                       { m_uniformTileRowHeightMinus1 = i; }
  int   getUniformTileRowHeightMinus1  ()                            { return m_uniformTileRowHeightMinus1; }
#endif
  void  setColumnWidth ( const std::vector<int>& columnWidth )       { m_tileColumnWidth = columnWidth; }
  uint32_t  getColumnWidth                 ( uint32_t columnIdx )            { return m_tileColumnWidth[columnIdx]; }
  void  setNumRowsMinus1               ( int i )                     { m_iNumRowsMinus1 = i; }
  int   getNumRowsMinus1               ()                            { return m_iNumRowsMinus1; }
  void  setRowHeight ( const std::vector<int>& rowHeight)            { m_tileRowHeight = rowHeight; }
  uint32_t  getRowHeight                   ( uint32_t rowIdx )               { return m_tileRowHeight[rowIdx]; }

  bool  getRectSliceFlag() const                                     { return m_rectSliceFlag; }
  void  setRectSliceFlag(bool val)                                   { m_rectSliceFlag = val; }
  int   getNumSlicesInPicMinus1() const                              { return m_numSlicesInPicMinus1; }
  void  setNumSlicesInPicMinus1(int val)                             { m_numSlicesInPicMinus1 = val; }
  int   getTopLeftBrickIdx(uint32_t columnIdx) const                 { return  m_topLeftBrickIdx[columnIdx]; }
  void  setTopLeftBrickIdx(const std::vector<int>& val)              { m_topLeftBrickIdx = val; }
  int   getBottomeRightBrickIdx(uint32_t columnIdx) const            { return  m_bottomRightBrickIdx[columnIdx]; }
  void  setBottomRightBrickIdx(const std::vector<int>& val)          { m_bottomRightBrickIdx = val; }
  bool  getLoopFilterAcrossSlicesEnabledFlag() const                 { return m_loopFilterAcrossSlicesEnabledFlag; }
  void  setLoopFilterAcrossSlicesEnabledFlag(bool val)               { m_loopFilterAcrossSlicesEnabledFlag = val; }
  bool  getSignalledSliceIdFlag() const                              { return m_signalledSliceIdFlag; }
  void  setSignalledSliceIdFlag(bool val)                            { m_signalledSliceIdFlag = val; }
  int   getSignalledSliceIdLengthMinus1() const                      { return m_signalledSliceIdLengthMinus1; }
  void  setSignalledSliceIdLengthMinus1(int val)                     { m_signalledSliceIdLengthMinus1 = val; }
  int   getSliceId(uint32_t columnIdx) const                         { return  m_sliceId[columnIdx]; }
  void  setSliceId(const std::vector<int>& val)                      { m_sliceId = val; }
  BrickSplitMap getBrickSplitMap() const                             { return  m_brickSplitMap; }
  void  setBrickSplitMap(const BrickSplitMap& val)                   { m_brickSplitMap = val; }

  void  xCheckGSParameters();
  void  setEntropyCodingSyncEnabledFlag(bool b)                      { m_entropyCodingSyncEnabledFlag = b; }
  bool  getEntropyCodingSyncEnabledFlag() const                      { return m_entropyCodingSyncEnabledFlag; }
  void  setDecodedPictureHashSEIType(HashType m)                     { m_decodedPictureHashSEIType = m; }
  HashType getDecodedPictureHashSEIType() const                      { return m_decodedPictureHashSEIType; }
  void  setBufferingPeriodSEIEnabled(bool b)                         { m_bufferingPeriodSEIEnabled = b; }
  bool  getBufferingPeriodSEIEnabled() const                         { return m_bufferingPeriodSEIEnabled; }
  void  setPictureTimingSEIEnabled(bool b)                           { m_pictureTimingSEIEnabled = b; }
  bool  getPictureTimingSEIEnabled() const                           { return m_pictureTimingSEIEnabled; }
#if JVET_O0041_FRAME_FIELD_SEI
  void  setFrameFieldInfoSEIEnabled(bool b)                           { m_frameFieldInfoSEIEnabled = b; }
  bool  getFrameFieldInfoSEIEnabled() const                           { return m_frameFieldInfoSEIEnabled; }
#endif
#if JVET_N0494_DRAP
  void  setDependentRAPIndicationSEIEnabled(bool b)                  { m_dependentRAPIndicationSEIEnabled = b; }
  int   getDependentRAPIndicationSEIEnabled() const                  { return m_dependentRAPIndicationSEIEnabled; }
#endif
#if HEVC_SEI
  void  setRecoveryPointSEIEnabled(bool b)                           { m_recoveryPointSEIEnabled = b; }
  bool  getRecoveryPointSEIEnabled() const                           { return m_recoveryPointSEIEnabled; }
  void  setToneMappingInfoSEIEnabled(bool b)                         { m_toneMappingInfoSEIEnabled = b;  }
  bool  getToneMappingInfoSEIEnabled()                               { return m_toneMappingInfoSEIEnabled;  }
  void  setTMISEIToneMapId(int b)                                    { m_toneMapId = b;  }
  int   getTMISEIToneMapId()                                         { return m_toneMapId;  }
  void  setTMISEIToneMapCancelFlag(bool b)                           { m_toneMapCancelFlag=b;  }
  bool  getTMISEIToneMapCancelFlag()                                 { return m_toneMapCancelFlag;  }
  void  setTMISEIToneMapPersistenceFlag(bool b)                      { m_toneMapPersistenceFlag = b;  }
  bool   getTMISEIToneMapPersistenceFlag()                           { return m_toneMapPersistenceFlag;  }
  void  setTMISEICodedDataBitDepth(int b)                            { m_codedDataBitDepth = b;  }
  int   getTMISEICodedDataBitDepth()                                 { return m_codedDataBitDepth;  }
  void  setTMISEITargetBitDepth(int b)                               { m_targetBitDepth = b;  }
  int   getTMISEITargetBitDepth()                                    { return m_targetBitDepth;  }
  void  setTMISEIModelID(int b)                                      { m_modelId = b;  }
  int   getTMISEIModelID()                                           { return m_modelId;  }
  void  setTMISEIMinValue(int b)                                     { m_minValue = b;  }
  int   getTMISEIMinValue()                                          { return m_minValue;  }
  void  setTMISEIMaxValue(int b)                                     { m_maxValue = b;  }
  int   getTMISEIMaxValue()                                          { return m_maxValue;  }
  void  setTMISEISigmoidMidpoint(int b)                              { m_sigmoidMidpoint = b;  }
  int   getTMISEISigmoidMidpoint()                                   { return m_sigmoidMidpoint;  }
  void  setTMISEISigmoidWidth(int b)                                 { m_sigmoidWidth = b;  }
  int   getTMISEISigmoidWidth()                                      { return m_sigmoidWidth;  }
  void  setTMISEIStartOfCodedInterva( int*  p )                      { m_startOfCodedInterval = p;  }
  int*  getTMISEIStartOfCodedInterva()                               { return m_startOfCodedInterval;  }
  void  setTMISEINumPivots(int b)                                    { m_numPivots = b;  }
  int   getTMISEINumPivots()                                         { return m_numPivots;  }
  void  setTMISEICodedPivotValue( int*  p )                          { m_codedPivotValue = p;  }
  int*  getTMISEICodedPivotValue()                                   { return m_codedPivotValue;  }
  void  setTMISEITargetPivotValue( int*  p )                         { m_targetPivotValue = p;  }
  int*  getTMISEITargetPivotValue()                                  { return m_targetPivotValue;  }
  void  setTMISEICameraIsoSpeedIdc(int b)                            { m_cameraIsoSpeedIdc = b;  }
  int   getTMISEICameraIsoSpeedIdc()                                 { return m_cameraIsoSpeedIdc;  }
  void  setTMISEICameraIsoSpeedValue(int b)                          { m_cameraIsoSpeedValue = b;  }
  int   getTMISEICameraIsoSpeedValue()                               { return m_cameraIsoSpeedValue;  }
  void  setTMISEIExposureIndexIdc(int b)                             { m_exposureIndexIdc = b;  }
  int   getTMISEIExposurIndexIdc()                                   { return m_exposureIndexIdc;  }
  void  setTMISEIExposureIndexValue(int b)                           { m_exposureIndexValue = b;  }
  int   getTMISEIExposurIndexValue()                                 { return m_exposureIndexValue;  }
  void  setTMISEIExposureCompensationValueSignFlag(bool b)           { m_exposureCompensationValueSignFlag = b;  }
  bool  getTMISEIExposureCompensationValueSignFlag()                 { return m_exposureCompensationValueSignFlag;  }
  void  setTMISEIExposureCompensationValueNumerator(int b)           { m_exposureCompensationValueNumerator = b;  }
  int   getTMISEIExposureCompensationValueNumerator()                { return m_exposureCompensationValueNumerator;  }
  void  setTMISEIExposureCompensationValueDenomIdc(int b)            { m_exposureCompensationValueDenomIdc =b;  }
  int   getTMISEIExposureCompensationValueDenomIdc()                 { return m_exposureCompensationValueDenomIdc;  }
  void  setTMISEIRefScreenLuminanceWhite(int b)                      { m_refScreenLuminanceWhite = b;  }
  int   getTMISEIRefScreenLuminanceWhite()                           { return m_refScreenLuminanceWhite;  }
  void  setTMISEIExtendedRangeWhiteLevel(int b)                      { m_extendedRangeWhiteLevel = b;  }
  int   getTMISEIExtendedRangeWhiteLevel()                           { return m_extendedRangeWhiteLevel;  }
  void  setTMISEINominalBlackLevelLumaCodeValue(int b)               { m_nominalBlackLevelLumaCodeValue = b;  }
  int   getTMISEINominalBlackLevelLumaCodeValue()                    { return m_nominalBlackLevelLumaCodeValue;  }
  void  setTMISEINominalWhiteLevelLumaCodeValue(int b)               { m_nominalWhiteLevelLumaCodeValue = b;  }
  int   getTMISEINominalWhiteLevelLumaCodeValue()                    { return m_nominalWhiteLevelLumaCodeValue;  }
  void  setTMISEIExtendedWhiteLevelLumaCodeValue(int b)              { m_extendedWhiteLevelLumaCodeValue =b;  }
  int   getTMISEIExtendedWhiteLevelLumaCodeValue()                   { return m_extendedWhiteLevelLumaCodeValue;  }
  void  setFramePackingArrangementSEIEnabled(bool b)                 { m_framePackingSEIEnabled = b; }
  bool  getFramePackingArrangementSEIEnabled() const                 { return m_framePackingSEIEnabled; }
  void  setFramePackingArrangementSEIType(int b)                     { m_framePackingSEIType = b; }
  int   getFramePackingArrangementSEIType()                          { return m_framePackingSEIType; }
  void  setFramePackingArrangementSEIId(int b)                       { m_framePackingSEIId = b; }
  int   getFramePackingArrangementSEIId()                            { return m_framePackingSEIId; }
  void  setFramePackingArrangementSEIQuincunx(int b)                 { m_framePackingSEIQuincunx = b; }
  int   getFramePackingArrangementSEIQuincunx()                      { return m_framePackingSEIQuincunx; }
  void  setFramePackingArrangementSEIInterpretation(int b)           { m_framePackingSEIInterpretation = b; }
  int   getFramePackingArrangementSEIInterpretation()                { return m_framePackingSEIInterpretation; }
  void  setSegmentedRectFramePackingArrangementSEIEnabled(bool b)    { m_segmentedRectFramePackingSEIEnabled = b; }
  bool  getSegmentedRectFramePackingArrangementSEIEnabled() const    { return m_segmentedRectFramePackingSEIEnabled; }
  void  setSegmentedRectFramePackingArrangementSEICancel(int b)      { m_segmentedRectFramePackingSEICancel = b; }
  int   getSegmentedRectFramePackingArrangementSEICancel()           { return m_segmentedRectFramePackingSEICancel; }
  void  setSegmentedRectFramePackingArrangementSEIType(int b)        { m_segmentedRectFramePackingSEIType = b; }
  int   getSegmentedRectFramePackingArrangementSEIType()             { return m_segmentedRectFramePackingSEIType; }
  void  setSegmentedRectFramePackingArrangementSEIPersistence(int b) { m_segmentedRectFramePackingSEIPersistence = b; }
  int   getSegmentedRectFramePackingArrangementSEIPersistence()      { return m_segmentedRectFramePackingSEIPersistence; }
  void  setDisplayOrientationSEIAngle(int b)                         { m_displayOrientationSEIAngle = b; }
  int   getDisplayOrientationSEIAngle()                              { return m_displayOrientationSEIAngle; }
  void  setTemporalLevel0IndexSEIEnabled(bool b)                     { m_temporalLevel0IndexSEIEnabled = b; }
  bool  getTemporalLevel0IndexSEIEnabled() const                     { return m_temporalLevel0IndexSEIEnabled; }
  void  setGradualDecodingRefreshInfoEnabled(bool b)                 { m_gradualDecodingRefreshInfoEnabled = b;    }
  bool  getGradualDecodingRefreshInfoEnabled() const                 { return m_gradualDecodingRefreshInfoEnabled; }
  void  setNoDisplaySEITLayer(int b)                                 { m_noDisplaySEITLayer = b;    }
  int   getNoDisplaySEITLayer()                                      { return m_noDisplaySEITLayer; }
#endif
#if JVET_N0867_TEMP_SCAL_HRD
  void  setBpDeltasGOPStructure(bool b)                              { m_bpDeltasGOPStructure = b;    }
  bool  getBpDeltasGOPStructure() const                              { return m_bpDeltasGOPStructure; }
#endif
  void  setDecodingUnitInfoSEIEnabled(bool b)                        { m_decodingUnitInfoSEIEnabled = b;    }
  bool  getDecodingUnitInfoSEIEnabled() const                        { return m_decodingUnitInfoSEIEnabled; }
#if HEVC_SEI
  void  setSOPDescriptionSEIEnabled(bool b)                          { m_SOPDescriptionSEIEnabled = b; }
  bool  getSOPDescriptionSEIEnabled() const                          { return m_SOPDescriptionSEIEnabled; }
  void  setScalableNestingSEIEnabled(bool b)                         { m_scalableNestingSEIEnabled = b; }
  bool  getScalableNestingSEIEnabled() const                         { return m_scalableNestingSEIEnabled; }
  void  setTMCTSSEIEnabled(bool b)                                   { m_tmctsSEIEnabled = b; }
  bool  getTMCTSSEIEnabled()                                         { return m_tmctsSEIEnabled; }
#endif
  void  setMCTSEncConstraint(bool b)                                 { m_MCTSEncConstraint = b; }
  bool  getMCTSEncConstraint()                                       { return m_MCTSEncConstraint; }
#if HEVC_SEI
  void  setTimeCodeSEIEnabled(bool b)                                { m_timeCodeSEIEnabled = b; }
  bool  getTimeCodeSEIEnabled()                                      { return m_timeCodeSEIEnabled; }
  void  setNumberOfTimeSets(int value)                               { m_timeCodeSEINumTs = value; }
  int   getNumberOfTimesets()                                        { return m_timeCodeSEINumTs; }
  void  setTimeSet(SEITimeSet element, int index)                    { m_timeSetArray[index] = element; }
  SEITimeSet &getTimeSet(int index)                                  { return m_timeSetArray[index]; }
  const SEITimeSet &getTimeSet(int index) const                      { return m_timeSetArray[index]; }
  void  setKneeSEIEnabled(int b)                                     { m_kneeSEIEnabled = b; }
  bool  getKneeSEIEnabled()                                          { return m_kneeSEIEnabled; }
  void  setKneeSEIId(int b)                                          { m_kneeSEIId = b; }
  int   getKneeSEIId()                                               { return m_kneeSEIId; }
  void  setKneeSEICancelFlag(bool b)                                 { m_kneeSEICancelFlag=b; }
  bool  getKneeSEICancelFlag()                                       { return m_kneeSEICancelFlag; }
  void  setKneeSEIPersistenceFlag(bool b)                            { m_kneeSEIPersistenceFlag = b; }
  bool  getKneeSEIPersistenceFlag()                                  { return m_kneeSEIPersistenceFlag; }
  void  setKneeSEIInputDrange(int b)                                 { m_kneeSEIInputDrange = b; }
  int   getKneeSEIInputDrange()                                      { return m_kneeSEIInputDrange; }
  void  setKneeSEIInputDispLuminance(int b)                          { m_kneeSEIInputDispLuminance = b; }
  int   getKneeSEIInputDispLuminance()                               { return m_kneeSEIInputDispLuminance; }
  void  setKneeSEIOutputDrange(int b)                                { m_kneeSEIOutputDrange = b; }
  int   getKneeSEIOutputDrange()                                     { return m_kneeSEIOutputDrange; }
  void  setKneeSEIOutputDispLuminance(int b)                         { m_kneeSEIOutputDispLuminance = b; }
  int   getKneeSEIOutputDispLuminance()                              { return m_kneeSEIOutputDispLuminance; }
  void  setKneeSEINumKneePointsMinus1(int b)                         { m_kneeSEINumKneePointsMinus1 = b; }
  int   getKneeSEINumKneePointsMinus1()                              { return m_kneeSEINumKneePointsMinus1; }
  void  setKneeSEIInputKneePoint(int *p)                             { m_kneeSEIInputKneePoint = p; }
  int*  getKneeSEIInputKneePoint()                                   { return m_kneeSEIInputKneePoint; }
  void  setKneeSEIOutputKneePoint(int *p)                            { m_kneeSEIOutputKneePoint = p; }
  int*  getKneeSEIOutputKneePoint()                                  { return m_kneeSEIOutputKneePoint; }
  void  setColourRemapInfoSEIFileRoot( const std::string &s )        { m_colourRemapSEIFileRoot = s; }
  const std::string &getColourRemapInfoSEIFileRoot() const           { return m_colourRemapSEIFileRoot; }
  void  setMasteringDisplaySEI(const SEIMasteringDisplay &src)       { m_masteringDisplay = src; }
#if U0033_ALTERNATIVE_TRANSFER_CHARACTERISTICS_SEI
  void  setSEIAlternativeTransferCharacteristicsSEIEnable( bool b)   { m_alternativeTransferCharacteristicsSEIEnabled = b;    }
  bool  getSEIAlternativeTransferCharacteristicsSEIEnable( ) const   { return m_alternativeTransferCharacteristicsSEIEnabled; }
  void  setSEIPreferredTransferCharacteristics(uint8_t v)              { m_preferredTransferCharacteristics = v;    }
  uint8_t getSEIPreferredTransferCharacteristics() const               { return m_preferredTransferCharacteristics; }
#endif
  void  setSEIGreenMetadataInfoSEIEnable( bool b)                    { m_greenMetadataInfoSEIEnabled = b;    }
  bool  getSEIGreenMetadataInfoSEIEnable( ) const                    { return m_greenMetadataInfoSEIEnabled; }
  void  setSEIGreenMetadataType(uint8_t v)                             { m_greenMetadataType = v;    }
  uint8_t getSEIGreenMetadataType() const                              { return m_greenMetadataType; }
  void  setSEIXSDMetricType(uint8_t v)                                 { m_xsdMetricType = v;    }
  uint8_t getSEIXSDMetricType() const                                  { return m_xsdMetricType; }

  const SEIMasteringDisplay &getMasteringDisplaySEI() const          { return m_masteringDisplay; }
#endif
  void         setUseWP               ( bool b )                     { m_useWeightedPred   = b;    }
  void         setWPBiPred            ( bool b )                     { m_useWeightedBiPred = b;    }
  bool         getUseWP               ()                             { return m_useWeightedPred;   }
  bool         getWPBiPred            ()                             { return m_useWeightedBiPred; }
  void         setLog2ParallelMergeLevelMinus2   ( uint32_t u )          { m_log2ParallelMergeLevelMinus2       = u;    }
  uint32_t         getLog2ParallelMergeLevelMinus2   ()                  { return m_log2ParallelMergeLevelMinus2;       }
  void         setMaxNumMergeCand                ( uint32_t u )          { m_maxNumMergeCand = u;      }
  uint32_t         getMaxNumMergeCand                ()                  { return m_maxNumMergeCand;   }
  void         setMaxNumAffineMergeCand          ( uint32_t u )      { m_maxNumAffineMergeCand = u;    }
  uint32_t     getMaxNumAffineMergeCand          ()                  { return m_maxNumAffineMergeCand; }
  void         setMaxNumTriangleCand             ( uint32_t u )      { m_maxNumTriangleCand = u;    }
  uint32_t     getMaxNumTriangleCand             ()                  { return m_maxNumTriangleCand; }
#if JVET_O0455_IBC_MAX_MERGE_NUM
  void         setMaxNumIBCMergeCand             ( uint32_t u )      { m_maxNumIBCMergeCand = u; }
  uint32_t     getMaxNumIBCMergeCand             ()                  { return m_maxNumIBCMergeCand; }
#endif
  void         setUseScalingListId    ( ScalingListMode u )          { m_useScalingListId       = u;   }
  ScalingListMode getUseScalingListId    ()                          { return m_useScalingListId;      }
  void         setScalingListFileName       ( const std::string &s ) { m_scalingListFileName = s;      }
  const std::string& getScalingListFileName () const                 { return m_scalingListFileName;   }
  void         setTMVPModeId ( int  u )                              { m_TMVPModeId = u;    }
  int          getTMVPModeId ()                                      { return m_TMVPModeId; }
#if JVET_O0238_PPS_OR_SLICE
  void         setConstantSliceHeaderParamsEnabledFlag ( bool u )    { m_constantSliceHeaderParamsEnabledFlag = u; }
  bool         getConstantSliceHeaderParamsEnabledFlag ()            { return m_constantSliceHeaderParamsEnabledFlag; }
  void         setPPSDepQuantEnabledIdc ( int u )                    { m_PPSDepQuantEnabledIdc = u; }
  int          getPPSDepQuantEnabledIdc ()                           { return m_PPSDepQuantEnabledIdc; }
  void         setPPSRefPicListSPSIdc0 ( int u )                     { m_PPSRefPicListSPSIdc0 = u; }
  int          getPPSRefPicListSPSIdc0 ()                            { return m_PPSRefPicListSPSIdc0; }
  void         setPPSRefPicListSPSIdc1 ( int u )                     { m_PPSRefPicListSPSIdc1 = u; }
  int          getPPSRefPicListSPSIdc1 ()                            { return m_PPSRefPicListSPSIdc1; }
  void         setPPSTemporalMVPEnabledIdc ( int u )                 { m_PPSTemporalMVPEnabledIdc = u; }
  int          getPPSTemporalMVPEnabledIdc ()                        { return m_PPSTemporalMVPEnabledIdc; }
  void         setPPSMvdL1ZeroIdc ( int u )                          { m_PPSMvdL1ZeroIdc = u; }
  int          getPPSMvdL1ZeroIdc ()                                 { return m_PPSMvdL1ZeroIdc; }
  void         setPPSCollocatedFromL0Idc ( int u )                   { m_PPSCollocatedFromL0Idc = u; }
  int          getPPSCollocatedFromL0Idc ()                          { return m_PPSCollocatedFromL0Idc; }
  void         setPPSSixMinusMaxNumMergeCandPlus1 ( uint32_t u )     { m_PPSSixMinusMaxNumMergeCandPlus1 = u; }
  uint32_t	   getPPSSixMinusMaxNumMergeCandPlus1 ()                 { return m_PPSSixMinusMaxNumMergeCandPlus1; }
  void         setPPSFiveMinusMaxNumSubblockMergeCandPlus1 ( uint32_t u ) { m_PPSFiveMinusMaxNumSubblockMergeCandPlus1 = u; }
  uint32_t     getPPSFiveMinusMaxNumSubblockMergeCandPlus1 ()        { return m_PPSFiveMinusMaxNumSubblockMergeCandPlus1; }
  void         setPPSMaxNumMergeCandMinusMaxNumTriangleCandPlus1 ( uint32_t u ) { m_PPSMaxNumMergeCandMinusMaxNumTriangleCandPlus1 = u; }
  uint32_t     getPPSMaxNumMergeCandMinusMaxNumTriangleCandPlus1 ()  { return m_PPSMaxNumMergeCandMinusMaxNumTriangleCandPlus1; }
#endif
  WeightedPredictionMethod getWeightedPredictionMethod() const       { return m_weightedPredictionMethod; }
  void         setWeightedPredictionMethod( WeightedPredictionMethod m ) { m_weightedPredictionMethod = m; }
  void         setDepQuantEnabledFlag( bool b )                      { m_DepQuantEnabledFlag = b;    }
  bool         getDepQuantEnabledFlag()                              { return m_DepQuantEnabledFlag; }
  void         setSignDataHidingEnabledFlag( bool b )                { m_SignDataHidingEnabledFlag = b;    }
  bool         getSignDataHidingEnabledFlag()                        { return m_SignDataHidingEnabledFlag; }
  bool         getUseRateCtrl         () const                       { return m_RCEnableRateControl;   }
  void         setUseRateCtrl         ( bool b )                     { m_RCEnableRateControl = b;      }
  int          getTargetBitrate       ()                             { return m_RCTargetBitrate;       }
  void         setTargetBitrate       ( int bitrate )                { m_RCTargetBitrate  = bitrate;   }
  int          getKeepHierBit         ()                             { return m_RCKeepHierarchicalBit; }
  void         setKeepHierBit         ( int i )                      { m_RCKeepHierarchicalBit = i;    }
  bool         getLCULevelRC          ()                             { return m_RCLCULevelRC; }
  void         setLCULevelRC          ( bool b )                     { m_RCLCULevelRC = b; }
  bool         getUseLCUSeparateModel ()                             { return m_RCUseLCUSeparateModel; }
  void         setUseLCUSeparateModel ( bool b )                     { m_RCUseLCUSeparateModel = b;    }
  int          getInitialQP           ()                             { return m_RCInitialQP;           }
  void         setInitialQP           ( int QP )                     { m_RCInitialQP = QP;             }
  bool         getForceIntraQP        ()                             { return m_RCForceIntraQP;        }
  void         setForceIntraQP        ( bool b )                     { m_RCForceIntraQP = b;           }
#if U0132_TARGET_BITS_SATURATION
  bool         getCpbSaturationEnabled()                             { return m_RCCpbSaturationEnabled;}
  void         setCpbSaturationEnabled( bool b )                     { m_RCCpbSaturationEnabled = b;   }
  uint32_t         getCpbSize             ()                             { return m_RCCpbSize;}
  void         setCpbSize             ( uint32_t ui )                    { m_RCCpbSize = ui;   }
  double       getInitialCpbFullness  ()                             { return m_RCInitialCpbFullness;  }
  void         setInitialCpbFullness  (double f)                     { m_RCInitialCpbFullness = f;     }
#endif
  bool         getTransquantBypassEnabledFlag()                      { return m_TransquantBypassEnabledFlag; }
  void         setTransquantBypassEnabledFlag(bool flag)             { m_TransquantBypassEnabledFlag = flag; }
  bool         getCUTransquantBypassFlagForceValue() const           { return m_CUTransquantBypassFlagForce; }
  void         setCUTransquantBypassFlagForceValue(bool flag)        { m_CUTransquantBypassFlagForce = flag; }
  CostMode     getCostMode( ) const                                  { return m_costMode; }
  void         setCostMode(CostMode m )                              { m_costMode = m; }

  void         setVPS(VPS *p)                                        { m_cVPS = *p; }
  VPS *        getVPS()                                              { return &m_cVPS; }
  void         setDPS(DPS *p)                                        { m_dps = *p; }
  DPS*         getDPS()                                              { return &m_dps; }
  void         setUseRecalculateQPAccordingToLambda (bool b)         { m_recalculateQPAccordingToLambda = b;    }
  bool         getUseRecalculateQPAccordingToLambda ()               { return m_recalculateQPAccordingToLambda; }

  void         setEfficientFieldIRAPEnabled( bool b )                { m_bEfficientFieldIRAPEnabled = b; }
  bool         getEfficientFieldIRAPEnabled( ) const                 { return m_bEfficientFieldIRAPEnabled; }

  void         setHarmonizeGopFirstFieldCoupleEnabled( bool b )      { m_bHarmonizeGopFirstFieldCoupleEnabled = b; }
  bool         getHarmonizeGopFirstFieldCoupleEnabled( ) const       { return m_bHarmonizeGopFirstFieldCoupleEnabled; }

#if HEVC_SEI
  void         setActiveParameterSetsSEIEnabled ( int b )            { m_activeParameterSetsSEIEnabled = b; }
  int          getActiveParameterSetsSEIEnabled ()                   { return m_activeParameterSetsSEIEnabled; }
#endif

  bool         getDecodingParameterSetEnabled()                      { return m_decodingParameterSetEnabled; }
  void         setDecodingParameterSetEnabled(bool i)                { m_decodingParameterSetEnabled = i; }
#if FIX_HRD_O0189
  bool         getHrdParametersPresentFlag()                         { return m_hrdParametersPresentFlag; }
  void         setHrdParametersPresentFlag(bool i)                   { m_hrdParametersPresentFlag = i; }
#endif
  bool         getVuiParametersPresentFlag()                         { return m_vuiParametersPresentFlag; }
  void         setVuiParametersPresentFlag(bool i)                   { m_vuiParametersPresentFlag = i; }
  bool         getAspectRatioInfoPresentFlag()                       { return m_aspectRatioInfoPresentFlag; }
  void         setAspectRatioInfoPresentFlag(bool i)                 { m_aspectRatioInfoPresentFlag = i; }
  int          getAspectRatioIdc()                                   { return m_aspectRatioIdc; }
  void         setAspectRatioIdc(int i)                              { m_aspectRatioIdc = i; }
  int          getSarWidth()                                         { return m_sarWidth; }
  void         setSarWidth(int i)                                    { m_sarWidth = i; }
  int          getSarHeight()                                        { return m_sarHeight; }
  void         setSarHeight(int i)                                   { m_sarHeight = i; }
  bool         getColourDescriptionPresentFlag()                     { return m_colourDescriptionPresentFlag; }
  void         setColourDescriptionPresentFlag(bool i)               { m_colourDescriptionPresentFlag = i; }
  int          getColourPrimaries()                                  { return m_colourPrimaries; }
  void         setColourPrimaries(int i)                             { m_colourPrimaries = i; }
  int          getTransferCharacteristics()                          { return m_transferCharacteristics; }
  void         setTransferCharacteristics(int i)                     { m_transferCharacteristics = i; }
  int          getMatrixCoefficients()                               { return m_matrixCoefficients; }
  void         setMatrixCoefficients(int i)                          { m_matrixCoefficients = i; }
  bool         getChromaLocInfoPresentFlag()                         { return m_chromaLocInfoPresentFlag; }
  void         setChromaLocInfoPresentFlag(bool i)                   { m_chromaLocInfoPresentFlag = i; }
  int          getChromaSampleLocTypeTopField()                      { return m_chromaSampleLocTypeTopField; }
  void         setChromaSampleLocTypeTopField(int i)                 { m_chromaSampleLocTypeTopField = i; }
  int          getChromaSampleLocTypeBottomField()                   { return m_chromaSampleLocTypeBottomField; }
  void         setChromaSampleLocTypeBottomField(int i)              { m_chromaSampleLocTypeBottomField = i; }
  int          getChromaSampleLocType()                              { return m_chromaSampleLocType; }
  void         setChromaSampleLocType(int i)                         { m_chromaSampleLocType = i; }
  bool         getOverscanInfoPresentFlag()                          { return m_overscanInfoPresentFlag; }
  void         setOverscanInfoPresentFlag(bool i)                    { m_overscanInfoPresentFlag = i; }
  bool         getOverscanAppropriateFlag()                          { return m_overscanAppropriateFlag; }
  void         setOverscanAppropriateFlag(bool i)                    { m_overscanAppropriateFlag = i; }
  bool         getVideoSignalTypePresentFlag()                       { return m_videoSignalTypePresentFlag; }
  void         setVideoSignalTypePresentFlag(bool i)                 { m_videoSignalTypePresentFlag = i; }
  bool         getVideoFullRangeFlag()                               { return m_videoFullRangeFlag; }
  void         setVideoFullRangeFlag(bool i)                         { m_videoFullRangeFlag = i; }

  bool         getProgressiveSourceFlag() const                      { return m_progressiveSourceFlag; }
  void         setProgressiveSourceFlag(bool b)                      { m_progressiveSourceFlag = b; }

  bool         getInterlacedSourceFlag() const                       { return m_interlacedSourceFlag; }
  void         setInterlacedSourceFlag(bool b)                       { m_interlacedSourceFlag = b; }

  bool         getNonPackedConstraintFlag() const                    { return m_nonPackedConstraintFlag; }
  void         setNonPackedConstraintFlag(bool b)                    { m_nonPackedConstraintFlag = b; }

  bool         getFrameOnlyConstraintFlag() const                    { return m_frameOnlyConstraintFlag; }
  void         setFrameOnlyConstraintFlag(bool b)                    { m_frameOnlyConstraintFlag = b; }

  uint32_t         getBitDepthConstraintValue() const                    { return m_bitDepthConstraintValue; }
  void         setBitDepthConstraintValue(uint32_t v)                    { m_bitDepthConstraintValue=v; }

  ChromaFormat getChromaFormatConstraintValue() const                { return m_chromaFormatConstraintValue; }
  void         setChromaFormatConstraintValue(ChromaFormat v)        { m_chromaFormatConstraintValue=v; }

  bool         getIntraConstraintFlag() const                        { return m_intraConstraintFlag; }
  void         setIntraConstraintFlag(bool b)                        { m_intraConstraintFlag=b; }

  bool         getOnePictureOnlyConstraintFlag() const               { return m_onePictureOnlyConstraintFlag; }
  void         setOnePictureOnlyConstraintFlag(bool b)               { m_onePictureOnlyConstraintFlag=b; }

  bool         getLowerBitRateConstraintFlag() const                 { return m_lowerBitRateConstraintFlag; }
  void         setLowerBitRateConstraintFlag(bool b)                 { m_lowerBitRateConstraintFlag=b; }

#if HEVC_SEI
  bool         getChromaResamplingFilterHintEnabled()                { return m_chromaResamplingFilterHintEnabled;}
  void         setChromaResamplingFilterHintEnabled(bool i)          { m_chromaResamplingFilterHintEnabled = i;}
  int          getChromaResamplingHorFilterIdc()                     { return m_chromaResamplingHorFilterIdc;}
  void         setChromaResamplingHorFilterIdc(int i)                { m_chromaResamplingHorFilterIdc = i;}
  int          getChromaResamplingVerFilterIdc()                     { return m_chromaResamplingVerFilterIdc;}
  void         setChromaResamplingVerFilterIdc(int i)                { m_chromaResamplingVerFilterIdc = i;}
#endif

  void         setSummaryOutFilename(const std::string &s)           { m_summaryOutFilename = s; }
  const std::string& getSummaryOutFilename() const                   { return m_summaryOutFilename; }
  void         setSummaryPicFilenameBase(const std::string &s)       { m_summaryPicFilenameBase = s; }
  const std::string& getSummaryPicFilenameBase() const               { return m_summaryPicFilenameBase; }

  void         setSummaryVerboseness(uint32_t v)                         { m_summaryVerboseness = v; }
  uint32_t         getSummaryVerboseness( ) const                        { return m_summaryVerboseness; }
  void         setIMV(int n)                                         { m_ImvMode = n; }
  int          getIMV() const                                        { return m_ImvMode; }
  void         setIMV4PelFast(int n)                                 { m_Imv4PelFast = n; }
  int          getIMV4PelFast() const                                { return m_Imv4PelFast; }
  void         setDecodeBitstream( int i, const std::string& s )     { m_decodeBitstreams[i] = s; }
  const std::string& getDecodeBitstream( int i )               const { return m_decodeBitstreams[i]; }
  bool         getForceDecodeBitstream1()                      const { return m_forceDecodeBitstream1; }
  void         setForceDecodeBitstream1( bool b )                    { m_forceDecodeBitstream1 = b; }
  void         setSwitchPOC( int i )                                 { m_switchPOC = i; }
  int          getSwitchPOC()                                  const { return m_switchPOC; }
  void         setSwitchDQP( int i )                                 { m_switchDQP = i; }
  int          getSwitchDQP()                                  const { return m_switchDQP; }
  void         setFastForwardToPOC( int i )                          { m_fastForwardToPOC = i; }
  int          getFastForwardToPOC()                           const { return m_fastForwardToPOC; }
  bool         useFastForwardToPOC()                           const { return m_fastForwardToPOC >= 0; }
  void         setStopAfterFFtoPOC( bool b )                         { m_stopAfterFFtoPOC = b; }
  bool         getStopAfterFFtoPOC()                           const { return m_stopAfterFFtoPOC; }
  void         setBs2ModPOCAndType( bool b )                         { m_bs2ModPOCAndType = b; }
  bool         getBs2ModPOCAndType()                           const { return m_bs2ModPOCAndType; }
  void         setDebugCTU( int i )                                  { m_debugCTU = i; }
  int          getDebugCTU()                                   const { return m_debugCTU; }

#if ENABLE_SPLIT_PARALLELISM
  void         setNumSplitThreads( int n )                           { m_numSplitThreads = n; }
  int          getNumSplitThreads()                            const { return m_numSplitThreads; }
  void         setForceSingleSplitThread( bool b )                   { m_forceSingleSplitThread = b; }
  int          getForceSingleSplitThread()                     const { return m_forceSingleSplitThread; }
#endif
#if ENABLE_WPP_PARALLELISM
  void         setNumWppThreads( int n )                             { m_numWppThreads = n; }
  int          getNumWppThreads()                              const { return m_numWppThreads; }
  void         setNumWppExtraLines( int n )                          { m_numWppExtraLines = n; }
  int          getNumWppExtraLines()                           const { return m_numWppExtraLines; }
  void         setEnsureWppBitEqual( bool b)                         { m_ensureWppBitEqual = b; }
  bool         getEnsureWppBitEqual()                          const { return m_ensureWppBitEqual; }
#endif
  void         setUseALF( bool b ) { m_alf = b; }
  bool         getUseALF()                                      const { return m_alf; }

#if JVET_O0756_CALCULATE_HDRMETRICS
  void        setWhitePointDeltaE( uint32_t index, double value )     { m_whitePointDeltaE[ index ] = value; }
  double      getWhitePointDeltaE( uint32_t index )             const { return m_whitePointDeltaE[ index ]; }
  void        setMaxSampleValue(double value)                         { m_maxSampleValue = value;}
  double      getMaxSampleValue()                               const { return m_maxSampleValue;}
  void        setSampleRange(int value)                               { m_sampleRange = static_cast<hdrtoolslib::SampleRange>(value);}
  hdrtoolslib::SampleRange getSampleRange()                     const { return m_sampleRange;}
  void        setColorPrimaries(int value)                            { m_colorPrimaries = static_cast<hdrtoolslib::ColorPrimaries>(value);}
  hdrtoolslib::ColorPrimaries getColorPrimaries()               const { return m_colorPrimaries;}
  void        setEnableTFunctionLUT(bool value)                       { m_enableTFunctionLUT = value;}
  bool        getEnableTFunctionLUT()                           const { return m_enableTFunctionLUT;}
  void        setChromaLocation(uint32_t index, int value)            { m_chromaLocation[ index ] = static_cast<hdrtoolslib::ChromaLocation>(value);}
  hdrtoolslib::ChromaLocation getChromaLocation(uint32_t index) const { return m_chromaLocation[index];}
  void        setChromaUPFilter(int value)                            { m_chromaUPFilter = value;}
  int         getChromaUPFilter()                               const { return m_chromaUPFilter;}
  void        setCropOffsetLeft(int value)                            { m_cropOffsetLeft = value;}
  int         getCropOffsetLeft()                               const { return m_cropOffsetLeft;}
  void        setCropOffsetTop(int value)                             { m_cropOffsetTop = value;}
  int         getCropOffsetTop()                                const { return m_cropOffsetTop;}
  void        setCropOffsetRight(int value)                           { m_cropOffsetRight = value;}
  int         getCropOffsetRight()                              const { return m_cropOffsetRight;}
  void        setCropOffsetBottom(int value)                          { m_cropOffsetBottom = value;}
  int         getCropOffsetBottom()                             const { return m_cropOffsetBottom;}
  void        setCalculateHdrMetrics(bool value)                      { m_calculateHdrMetrics = value;}
  bool        getCalcluateHdrMetrics()                          const { return m_calculateHdrMetrics;}
#endif

#if JVET_O1164_RPR
  void        setScalingRatio( double hor, double ver )              { m_scalingRatioHor = hor, m_scalingRatioVer = ver;  }
  void        setRPREnabled( bool b )                                { m_rprEnabled = b;    }
  bool        isRPREnabled()                                   const { return m_rprEnabled; }
  void        setSwitchPocPeriod( int p )                            { m_switchPocPeriod = p;}
  void        setUpscaledOutput( int b )                             { m_upscaledOutput = b; }
  int         getUpscaledOutput()                              const { return m_upscaledOutput; }
#endif
};

//! \}

#endif // !defined(AFX_TENCCFG_H__6B99B797_F4DA_4E46_8E78_7656339A6C41__INCLUDED_)
