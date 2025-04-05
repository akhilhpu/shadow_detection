#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Structure to represent a bounding box (x_min, y_min, x_max, y_max)
typedef struct {
float x1;
float y1;
float x2;
float y2;
} BoundingBox;

// Helper function to calculate the Intersection over Union (IoU) of two bounding boxes
float box_iou(BoundingBox box1, BoundingBox box2) {
float x_left = fmax(box1.x1, box2.x1);
float y_top = fmax(box1.y1, box2.y1);
float x_right = fmin(box1.x2, box2.x2);
float y_bottom = fmin(box1.y2, box2.y2);

if (x_right < x_left || y_bottom < y_top) {
return 0.0f; // No intersection
}

float intersection_area = (x_right - x_left) * (y_bottom - y_top);
float area1 = (box1.x2 - box1.x1) * (box1.y2 - box1.y1);
float area2 = (box2.x2 - box2.x1) * (box2.y2 - box2.y1);
float union_area = area1 + area2 - intersection_area;

if (union_area <= 0) {
return 0.0f; // Avoid division by zero
}

return intersection_area / union_area;
}

// Helper function to calculate the squared Euclidean distance between two points
float euclidean_distance_squared(float x1, float y1, float x2, float y2) {
return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
}

// Helper function to find the minimum of two floats
float float_min(float a, float b) {
return (a < b) ? a : b;
}

// Helper function to find the maximum of two floats
float float_max(float a, float b) {
return (a > b) ? a : b;
}

// Modified CIoU loss function
float ciou_loss_modified(BoundingBox* pred_boxes, BoundingBox* target_boxes, int num_boxes, float w_center, float w_aspect) {
if (num_boxes == 0) {
return 0.0f;
}

float total_loss = 0.0f;
for (int i = 0; i < num_boxes; ++i) {
float iou = box_iou(pred_boxes[i], target_boxes[i]);

// Center distance penalty
float center_pred_x = (pred_boxes[i].x1 + pred_boxes[i].x2) / 2.0f;
float center_pred_y = (pred_boxes[i].y1 + pred_boxes[i].y2) / 2.0f;
float center_gt_x = (target_boxes[i].x1 + target_boxes[i].x2) / 2.0f;
float center_gt_y = (target_boxes[i].y1 + target_boxes[i].y2) / 2.0f;
float dist_center_sq = euclidean_distance_squared(center_pred_x, center_pred_y, center_gt_x, center_gt_y);

// Smallest enclosing box diagonal squared
float min_x = float_min(pred_boxes[i].x1, target_boxes[i].x1);
float min_y = float_min(pred_boxes[i].y1, target_boxes[i].y1);
float max_x = float_max(pred_boxes[i].x2, target_boxes[i].x2);
float max_y = float_max(pred_boxes[i].y2, target_boxes[i].y2);
float diag_enclosing_sq = euclidean_distance_squared(min_x, min_y, max_x, max_y) + 1e-7f; // Avoid division by zero

float center_penalty = dist_center_sq / diag_enclosing_sq;

// Aspect ratio penalty (example: direct difference)
float pred_w = pred_boxes[i].x2 - pred_boxes[i].x1;
float pred_h = pred_boxes[i].y2 - pred_boxes[i].y1;
float gt_w = target_boxes[i].x2 - target_boxes[i].x1;
float gt_h = target_boxes[i].y2 - target_boxes[i].y1;
float ar_pred = pred_w / (pred_h + 1e-7f);
float ar_gt = gt_w / (gt_h + 1e-7f);
float aspect_ratio_penalty = fabs(ar_pred - ar_gt);

float ciou = iou - center_penalty - w_aspect * aspect_ratio_penalty;
float loss = 1.0f - ciou;
total_loss += loss;
}

return total_loss / num_boxes;
}

int main() {
// Example usage:
int num_predictions = 2;
BoundingBox* predictions = (BoundingBox*)malloc(num_predictions * sizeof(BoundingBox));
BoundingBox* targets = (BoundingBox*)malloc(num_predictions * sizeof(BoundingBox));

// Prediction 1
predictions[0].x1 = 10.0f;
predictions[0].y1 = 10.0f;
predictions[0].x2 = 30.0f;
predictions[0].y2 = 30.0f;
// Target 1
targets[0].x1 = 15.0f;
targets[0].y1 = 15.0f;
targets[0].x2 = 35.0f;
targets[0].y2 = 35.0f;

// Prediction 2
predictions[1].x1 = 50.0f;
predictions[1].y1 = 50.0f;
predictions[1].x2 = 70.0f;
predictions[1].y2 = 70.0f;
// Target 2
targets[1].x1 = 55.0f;
targets[1].y1 = 52.0f;
targets[1].x2 = 75.0f;
targets[1].y2 = 78.0f;

float center_weight = 1.5f;
float aspect_weight = 0.8f;
float loss = ciou_loss_modified(predictions, targets, num_predictions, center_weight, aspect_weight);

printf("Modified CIoU Loss: %f\n", loss);

free(predictions);
free(targets);

return 0;
}