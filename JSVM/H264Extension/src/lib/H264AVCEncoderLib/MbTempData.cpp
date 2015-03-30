
#include "H264AVCEncoderLib.h"

#include "MbTempData.h"

#include "H264AVCCommonLib/MbData.h"
#include "H264AVCCommonLib/YuvMbBuffer.h"



H264AVC_NAMESPACE_BEGIN


ErrVal IntMbTempData::init( MbDataAccess& rcMbDataAccess )
{                                                   //���ָ�벻���ڣ��򿪱��ڴ� ���ú���Ĺ��캯����ʼ��  �������ԭָ�� ��placement newȥ��ʼ��
  m_pcMbDataAccess = new( m_pcMbDataAccess ) MbDataAccess( rcMbDataAccess, *this );     //������operator new()  ���� rcMbDataAccess �� ��ǰ this��������ݳ�ʼ�� m_pcMbDataAccess
  clear();
  return Err::m_nOK;
}

ErrVal IntMbTempData::uninit()
{
  return Err::m_nOK;
}


IntMbTempData::IntMbTempData() :
m_pcMbDataAccess( NULL )
{
  m_pcMbDataAccess = NULL;
  clear();

  MbData::init( this, &m_acMbMvdData[0], &m_acMbMvdData[1], &m_acMbMotionData[0], &m_acMbMotionData[1] );
}


IntMbTempData::~IntMbTempData()
{
  delete m_pcMbDataAccess;
  m_pcMbDataAccess = NULL;
}


Void IntMbTempData::clear()       //�Ѹ������б���(�̳ж�����)���
{
  MbData::clear();
  YuvMbBuffer::setZero();           // ��� m_aucYuvBuffer

  MbDataStruct::clear();
  CostData::clear();
  MbTransformCoeffs::clear();          //���ñ任������ͷ��������任��ϵ��
}



Void IntMbTempData::clearCost()
{
  CostData::clear();
}



Void IntMbTempData::copyTo( MbDataAccess& rcMbDataAccess )
{
  rcMbDataAccess.getMbData()            .copyFrom( *this );      //�ѵ�ǰ IntMbTempData�� MbDataStruct�ṹ���� => rcMbDataAccess �� MbData����
  rcMbDataAccess.getMbTCoeffs()         .copyFrom( *this );    //�ѵ�ǰ IntMbTempData�� MbTransformCoeffs�ṹ���� => rcMbDataAccess �� MbTransformCoeffs����

  rcMbDataAccess.getMbMvdData(LIST_0)   .copyFrom( m_acMbMvdData[LIST_0] );       //���� Motion �� MV ��Ϣ
  rcMbDataAccess.getMbMotionData(LIST_0).copyFrom( m_acMbMotionData[LIST_0] );

  if( rcMbDataAccess.getSH().isBSlice() )
  {
    rcMbDataAccess.getMbMvdData(LIST_1)   .copyFrom( m_acMbMvdData[LIST_1] );
    rcMbDataAccess.getMbMotionData(LIST_1).copyFrom( m_acMbMotionData[LIST_1] );
  }
}


Void IntMbTempData::copyResidualDataTo( MbDataAccess& rcMbDataAccess )
{
  rcMbDataAccess.getMbData    ().setBCBP              ( getBCBP             () );
  rcMbDataAccess.getMbData    ().setMbExtCbp          ( getMbExtCbp         () );
  rcMbDataAccess.getMbData    ().setQp                ( getQp               () );
  rcMbDataAccess.getMbData    ().setQp4LF             ( getQp4LF            () );
  rcMbDataAccess.getMbTCoeffs ().copyFrom             ( *this                  );
  rcMbDataAccess.getMbData    ().setTransformSize8x8  ( isTransformSize8x8  () );
  rcMbDataAccess.getMbData    ().setResidualPredFlag  ( getResidualPredFlag () );
}


Void IntMbTempData::loadChromaData( IntMbTempData& rcMbTempData )
{
  memcpy( get(CIdx(0)), rcMbTempData.get(CIdx(0)), sizeof(TCoeff)*128);
  setChromaPredMode( rcMbTempData.getChromaPredMode() );
  YuvMbBuffer::loadChroma( rcMbTempData );
  distU()  = rcMbTempData.distU();
  distV()  = rcMbTempData.distV();
  getTempYuvMbBuffer().loadChroma( rcMbTempData.getTempYuvMbBuffer() );
}


H264AVC_NAMESPACE_END
