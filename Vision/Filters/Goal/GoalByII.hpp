#pragma once

#include "Filters/Filter.hpp"

namespace Vision {
namespace Filters {

/// Uses an algorithm based on the approach proposed by Berlin United (spl team)
/// at RoHOW 2017 to identify the best 'n' regions of interest
///
/// Input:
/// - WhiteII: An integral image of (rows + 1, cols + 1) monochannel and at least 32 bits
/// - GreenII: An integral image of (rows + 1, cols + 1) monochannel and at least 32 bits
/// - PostWidth: An image of (rows, cols) monochannel (CV_32F)
/// - FieldBorder: An image of (maskScale*rows,maskScale*cols) with binary content
//                 (used only if useMask>0)
/// Output:
/// - An image of (rows, cols) where pixels are colored in blue or red depending on their score
///
/// Three different types of patches are considered
/// - Above (width approximately of the post)
/// - Below (width approximately of the post)
/// - Boundary (top is top of above, bottom is bottom of below, width is larger)
class GoalByII : public Filter {
public:
  GoalByII();

  virtual std::string getClassName() const override;
  virtual int expectedDependencies() const override;

protected:
  virtual void process() override;

  virtual void setParameters() override;

  cv::Mat getHeatMap(const cv::Mat & scores, double minScore, double maxScore) const;

private:
  /// Return the patch associated to the above rect of the goal at the given point
  cv::Rect_<float> getAbovePatch(int x, int y, float width);

  /// Return the patch associated to the above right rect of the goal at the given point
  cv::Rect_<float> getAboveRightPatch(int x, int y, float width);

  /// Return the patch associated to the above left rect of the goal at the given point
  cv::Rect_<float> getAboveLeftPatch(int x, int y, float width);

  /// Return the patch associated to the below rect at the given point
  cv::Rect_<float> getBelowPatch(int x, int y, float width);

  /// Return the patch associated to the boundary rect at the given point
  cv::Rect_<float> getBoundaryPatch(int x, int y, float width);

  /// Return the patch associated to the roi rect at the given point
  cv::Rect_<float> getROIPatch(int x, int y, float width);

  /// Return the score of the patch given the provided integralImage
  double getPatchScore(const cv::Rect & patch,
                       const cv::Mat & whiteII, const cv::Mat & greenII);

  /// img should be CV_32SC1
  /// draw pixels in [start_x,end_x[ * [start_y,end_y[
  void fillScore(cv::Mat & img, int score,
                 int start_x, int end_x,
                 int start_y, int end_y);

  /// Number of rows in the result image
  int rows;

  /// Number of columns in the result image;
  int cols;

  /// A ratio used to increase width size
  ParamFloat widthScale;

  /// Inner patch above point: aboveRatio * width * widthScale
  ParamFloat aboveRatio;

  /// Boundary patch below point: belowRatio * width * widthScale
  ParamFloat belowRatio;

  /// Boundary width: boundaryWidthRatio * width * widthScale
  ParamFloat boundaryWidthRatio;

  /// Size of the ROI: roiRatio * width * widthScale (squared, centered)
  ParamFloat roiRatio;

  /// Minimal expected width (after widthScale) to accept the region
  ParamFloat minWidth;

  /// Minimal score for considering that region of interest is acceptable
  /// Reminder: score is in [-510,510]
  ParamFloat minScore;

  /// Maximal number of ROI generated by the filter
  ParamInt maxRois;

  // What is the weight of (above - below)
  ParamFloat belowCoeff;
  // What is the weight of (2*above - above_right - above_left)
  ParamFloat sideCoeff;

  /// Since using a high resolution Integral Image helps to detect balls which
  /// are far away, a decimationRate is used to compute the score function only
  /// at an interval given by decimationRate
  ParamInt decimationRate;

  /// 0: No heatMap produced
  ParamInt tagLevel;

  /// 0 : no use of provided mask
  /// 1+: use mask
  ParamInt useMask;

  /// Minimal radius when mask is used but entirely filled
  ParamFloat filledMaskMinWidth;
};
}
}


