set(the_description "Face recognition etc")
ocv_define_module(face opencv_core
    opencv_imgproc
    opencv_objdetect
    opencv_calib3d   # estimateAffinePartial2D() (trainFacemark)
    opencv_photo     # seamlessClone() (face_swap sample)
    WRAP python java objc
)
# NOTE: objdetect module is needed for one of the samples

