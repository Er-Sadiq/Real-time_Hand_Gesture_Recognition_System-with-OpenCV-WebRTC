#include <opencv2/opencv.hpp>
#include <mediapipe/framework/port/opencv_imgproc_inc.h>
#include <mediapipe/framework/port/opencv_highgui_inc.h>
#include <mediapipe/framework/port/statusor.h>
#include <mediapipe/framework/port/status.h>
#include <mediapipe/framework/formats/classification.pb.h>
#include <mediapipe/framework/formats/image_frame.h>
#include <mediapipe/framework/formats/image_frame_opencv.h>
#include <mediapipe/framework/calculator_framework.h>
#include <mediapipe/framework/formats/landmark.pb.h>
#include <mediapipe/framework/formats/landmark_list.pb.h>
#include <mediapipe/modules/hand_landmark/palm_detection.h>
#include <mediapipe/modules/hand_landmark/hand_landmark.h>

using namespace mediapipe;

int main() {
    // Initialize MediaPipe Hands modules.
    Hands hand_tracking;

    // Initialize OpenCV VideoCapture for camera input.
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Error: Cannot open camera." << std::endl;
        return -1;
    }

    while (true) {
        cv::Mat frame;
        cap >> frame;
        if (frame.empty()) {
            std::cerr << "Error: Cannot read frame from camera." << std::endl;
            break;
        }

        // Convert OpenCV Mat to MediaPipe ImageFrame.
        ImageFrame image_frame = cv::MatToImageFrame(frame, ImageFormat::SRGB);

        // Process the frame with MediaPipe Hands.
        StatusOr<hand_landmark::LandmarkList> hand_landmarks_or =
            hand_tracking.Process(image_frame);
        if (hand_landmarks_or.ok()) {
            // Draw hand landmarks on the frame.
            const auto& hand_landmarks = hand_landmarks_or.value();
            for (const auto& landmark : hand_landmarks.landmark()) {
                // Draw landmarks on the frame (you can customize the drawing here).
                // For example, draw a circle at each landmark position.
                int x = static_cast<int>(landmark.x() * frame.cols);
                int y = static_cast<int>(landmark.y() * frame.rows);
                cv::circle(frame, cv::Point(x, y), 5, cv::Scalar(0, 255, 0), -1);
            }
        } else {
            std::cerr << "Error: Failed to process hand landmarks." << std::endl;
        }

        // Display the frame with hand landmarks.
        cv::imshow("Hand Gesture Recognition", frame);

        // Press 'q' to exit the loop.
        if (cv::waitKey(1) == 'q') {
            break;
        }
    }

    // Release the VideoCapture and close all OpenCV windows.
    cap.release();
    cv::destroyAllWindows();

    return 0;
}
