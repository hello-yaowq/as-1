#! /bin/python3

# https://github.com/tensorflow/models/blob/master/research/slim/nets/alexnet.py
import sys,os
import argparse
import tensorflow as tf
import numpy as np
import glob
import random


parser = argparse.ArgumentParser()
parser.add_argument('-m', dest='model', default='v2', help='specify the model name < v2, or a *pb file >')

options = parser.parse_args()

if(not os.path.exists('./flower_photos')):
    os.system('wget http://download.tensorflow.org/example_images/flower_photos.tgz')
    os.system('tar xf flower_photos.tgz')

class_names = ['daisy', 'dandelion', 'roses', 'sunflowers', 'tulips']

class TrainingDataset():
    def __init__(self):
        self.datasets = []
        for label,cls in enumerate(class_names):
            for imgfile in glob.glob('flower_photos/%s/*.jpg'%(cls)):
                index = random.randint(0,len(self.datasets))
                self.datasets.insert(index, (imgfile,label))
        self.pos = 0
        self.sizes = len(self.datasets)
        print('dataset size is', self.sizes)

    def next_batch(self, batchsz):
        # https://blog.csdn.net/zsean/article/details/76383100
        # https://github.com/tensorflow/hub/blob/master/examples/image_retraining/retrain.py
        X = []
        Y = []
        if(batchsz > (self.sizes-self.pos)):
            batchsz = self.sizes - self.pos
        for id,(imgfile,label) in enumerate(self.datasets[self.pos:self.pos+batchsz]):
            #print('loading', id, imgfile)
            image_raw_data = tf.gfile.GFile(imgfile,'rb').read()
            img_data = tf.image.decode_jpeg(image_raw_data)
            img = tf.image.resize_images(img_data, [224,224],method=0)
            X.append(img.eval())
            y =  np.zeros(len(class_names))
            y[label] = 1.0
            Y.append(y)
        self.pos += batchsz
        if(self.pos >= self.sizes):
            self.pos = 0
            random.shuffle(self.datasets)
        return X,Y

class TrainingDataset2():
    # https://zhuanlan.zhihu.com/p/30751039
    # https://zhuanlan.zhihu.com/p/27238630
    def __init__(self):
        pass
        # TODO: how to use tf Dataset queue reader

def alexnet_slim_model():
    from tensorflow.contrib.slim import nets
    x = tf.placeholder(tf.float32, [None, 224,224,3], name='x')
    y_ = tf.placeholder(tf.float32, [None, len(class_names)], name='y_')
    model,end_points= nets.alexnet.alexnet_v2(x,len(class_names))
    return x,end_points['alexnet_v2/fc8'],y_

def train_alexnet(x, y, y_):
    datasets = TrainingDataset()
    cross_entropy = tf.reduce_mean(-tf.reduce_sum(y_ * tf.log(y), reduction_indices=[1]))
    train_step = tf.train.GradientDescentOptimizer(0.5).minimize(cross_entropy)
    correct_prediction = tf.equal(tf.argmax(y,1), tf.argmax(y_,1))
    accuracy = tf.reduce_mean(tf.cast(correct_prediction, tf.float32))
    sess.run(tf.global_variables_initializer())
    for i in range(10000):
        batch_xs, batch_ys = datasets.next_batch(100)
        sess.run(train_step, feed_dict={x: batch_xs, y_: batch_ys})
        if(i%100 == 0):
            train_accuracy = accuracy.eval(feed_dict={x:batch_xs, y_: batch_ys})
            print("step %d, training accuracy %g"%(i, train_accuracy))
    return x,y,y_

sess = tf.InteractiveSession()

x,y,y_ = alexnet_slim_model()

train_alexnet(x,y,y_)


