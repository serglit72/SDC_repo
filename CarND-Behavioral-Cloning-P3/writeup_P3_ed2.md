#**Behavioral Cloning** 


My project includes the following files:
* `model.py` containing the script to create and train the model
* `drive.py` for driving the car in autonomous mode
* `model.h5` containing a trained convolution neural network 
* `run2.mp4` video captured a 1 loop of driving car in autonomous mode
* `writeup_report.md`  summarizing the results

####**2. Submission includes functional code**
Using the Udacity provided simulator and my drive.py file, the car can be driven autonomously around the track by executing 
```sh
python drive.py model.h5 model.py run2.mp4
```

####**3. Submission code is usable and readable**

The model.py file contains the code for training and saving the convolution neural network. The file shows the pipeline I used for training and validating the model, and it contains comments to explain how the code works.

###**Model Architecture and Training Strategy**

####**1. An appropriate model architecture has been employed.** 

My model consists of a convolution neural network with 3x3 filter sizes and depths between **24 and 128** (`model.py` lines 45-62) 

The model includes `RELU` layers to introduce nonlinearity (code line 45), and the data is normalized in the model using a Keras `lambda` layer (code line 46). 
Then I cropped the images inorder to decrease amount of data and speed up data processing (code line 47).

####**2. Attempts to reduce overfitting in the model**
 
My model  got acceptable results on 3rd epoch. Overfitting becomes on 4th and further epochs. So I played with the number of epochs 
And also I reduced validation data from 20% to 15% that gave me to diminish loss coefficient.
The model was trained and validated on different data sets . The model was tested 
by running it through the simulator and ensuring that the vehicle could stay on the track.

####**3. Model parameter tuning**

The model used an `adam` optimizer, but the learning rate is available to adjust it manually (`model.py` line 66). I used `optimizer = Adam(lr=0.0001).`  But the numbers of epoch are tunned. I tried 5,7,10 epochs for training but I hadn't notable improvements. So the acceptable results was reached on 3 epochs.
  
####**4. Appropriate training data**

Training data was chosen to keep the vehicle driving on the road. I used a combination of center lane driving, recovering from the left and right sides of the road. I manually drove and tried to keep central line. After several attempts my skill has improved and I was able to drive without droping off the lane.

For details about how I created the training data, see the next section. 

###**Model Architecture and Training Strategy**

####**1. Solution Design Approach**

The overall strategy for deriving a model architecture was to train neural network and optimize of the training data.

My first step was to use a convolution neural network model similar to the LeNet algorithm. I thought this model might be appropriate because of that model provides regression algorithm image processing. 

In order to gauge how well the model was working, I split my image and steering angle data into a training and validation set. I found that my first model had a low mean squared error on the training set but a high mean squared error on the validation set. This implied that the model was overfitting. 

To combat the overfitting, I cropped the image and focused on bottom half of the image. Also I used 3 cameras position to improve accuracy.

Then I modified the model so that number of `epoch=3`

The final step was to run the simulator to see how well the car was driving around track one. 

At the end of the process, the vehicle is able to drive autonomously around the track without leaving the road.

####**2. Final Model Architecture**

The final model architecture (model.py lines 46-61) consisted of a convolution neural network with the following layers and layer sizes 24,32,64,128...
`2D convolution` layer.This layer creates a convolution kernel that is convolved with the layer input to produce a tensor of outputs.  

When using this layer as the first layer in a model, provide the keyword argument `input_shape` (tuple of integers). So `input_shape=(320, 160, 3)` for `320x160 RGB` pictures. **Very important to convert BGR image are gotten by cv2.read function from cv2 library into RGB image.** This point was really important to provide image compatibility.
To add accuracy to my model I used augmented images. In order to get it I apply  image augmentation with flip technique (line 27-35).



####**3. Creation of the Training Set & Training Process**

I used my own data for training the model.  

To capture good driving behavior, I've driven 3 loops without falling out. Here is an example image :

![image1](/images/right_2017_10_24_21_14_20_690.png)
![image2](/images/left_2017_10_24_21_14_20_902.png)
![image3](/images/center_2017_10_24_21_14_20_902.png)

I  recorded the vehicle goes like a drifting style driving from the left side and right sides of the road back to center so that the vehicle would learn to correct behavior in that cases.

Then I repeated this process on track two and three in order to get more data points.

I tried to augment the data set, I also flipped images and angles. 

After the collection process, I had 17K+ number of data points. I then preprocessed this data by crop, normalizing and convert BRG to RGB images and, also flip images.

I finally randomly shuffled the data set and put 15% of the data into a validation set. 

I used this training data for training the model. The validation set helped determine if the model was over or under fitting. The ideal number of epochs was 3 as evidenced by acceptable accuracy  I used an adam optimizer so that manually training the learning rate wasn't necessary.
