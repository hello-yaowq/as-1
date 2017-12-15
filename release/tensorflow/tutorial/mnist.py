#! /bin/python3

# according to https://www.tensorflow.org/get_started/mnist/beginners
import os
import sys

# download of MNIST data
if(not os.path.exists('train-images-idx3-ubyte.gz')):
    os.system('wget http://yann.lecun.com/exdb/mnist/train-images-idx3-ubyte.gz')
    os.system('wget http://yann.lecun.com/exdb/mnist/train-labels-idx1-ubyte.gz')
    os.system('wget http://yann.lecun.com/exdb/mnist/t10k-images-idx3-ubyte.gz')
    os.system('wget http://yann.lecun.com/exdb/mnist/t10k-labels-idx1-ubyte.gz')

# hack of the MNIST data 
def extract_mnist_image(image, label, p, limit=1000):
    if(not os.path.exists(p)): os.mkdir(p)
    with gzip.open(image,'rb') as f:
        def U32(b):
            return (b[0]<<24)+(b[1]<<16)+(b[2]<<8)+(b[3]<<0)
        def RGB(b):
            if(0 == b[0]):
                return 0xFFFFFFFF
            return b[0]
        f2 = gzip.open(label,'rb')
        magic = U32(f.read(4))
        print('magic', magic)
        number = U32(f.read(4))
        print('number of images', number)
        raws = U32(f.read(4))
        print('raws', raws)
        cols = U32(f.read(4))
        print('coloumns', cols)
        magic = U32(f2.read(4))
        print('magic', magic)
        number2 = U32(f2.read(4))
        print('number of labels', number2)
        assert(number == number2)
        for i in range(number):
            img = QImage(raws,cols,QImage.Format_RGB32)
            for w in range(raws):
                for h in range(cols):
                    rgb = RGB(f.read(1))
                    img.setPixel(w,h,rgb)
            iname = '%s/img%d_%d.png'%(p,i,f2.read(1)[0])
            img.save(iname,'PNG')
            if(i > limit):
                print('too many images, reach the limit %d, stop!'%(limit))
                break
        f2.close()
        print('convert MNIST data to image under directory %s done!'%(p));

if(not os.path.exists('tmp')):
    import gzip
    from PyQt5.QtWidgets import QApplication
    from PyQt5.QtGui import QImage
    app = QApplication(sys.argv)
    os.mkdir('tmp')
    extract_mnist_image('train-images-idx3-ubyte.gz', 'train-labels-idx1-ubyte.gz', 'tmp/train')
    extract_mnist_image('t10k-images-idx3-ubyte.gz', 't10k-labels-idx1-ubyte.gz', 'tmp/test')

from tensorflow.examples.tutorials.mnist import input_data
mnist = input_data.read_data_sets("./", one_hot=True)

import tensorflow as tf

x = tf.placeholder(tf.float32, [None, 784])
W = tf.Variable(tf.zeros([784, 10]))
b = tf.Variable(tf.zeros([10]))
y = tf.nn.softmax(tf.matmul(x, W) + b)
y_ = tf.placeholder(tf.float32, [None, 10])
cross_entropy = tf.reduce_mean(-tf.reduce_sum(y_ * tf.log(y), reduction_indices=[1]))
train_step = tf.train.GradientDescentOptimizer(0.5).minimize(cross_entropy)
sess = tf.InteractiveSession()
tf.global_variables_initializer().run()
for _ in range(1000):
    batch_xs, batch_ys = mnist.train.next_batch(100)
    sess.run(train_step, feed_dict={x: batch_xs, y_: batch_ys})

correct_prediction = tf.equal(tf.argmax(y,1), tf.argmax(y_,1))
accuracy = tf.reduce_mean(tf.cast(correct_prediction, tf.float32))
print(sess.run(accuracy, feed_dict={x: mnist.test.images, y_: mnist.test.labels}))

index = 10
while(index >= 0):
    from PyQt5.QtWidgets import QApplication
    from PyQt5.QtGui import QImage
    app = QApplication(sys.argv)
    index = input('index of test image of MNIST:')
    index = int(index)
    if(index < 0): break
    # save the test image to png file for check
    img = QImage(28,28,QImage.Format_RGB32)
    for i,b in enumerate(mnist.test.images[index]):
        def RGB(b):
            if(0 == b):
                return 0xFFFFFFFF
            return int((b*255))&0xFF
        rgb = RGB(b)
        img.setPixel(int(i/28),i%28,rgb)
        iname = 'test.png'
        img.save(iname,'PNG')

    a = sess.run(y, feed_dict={x: mnist.test.images[index:index+1]})
    for i,p in enumerate(a[0]):
        if(p> 0.5):
            fix = 'Y'
        else:
            fix = 'N'
        print('[%d](./test.png) is %02d%% percent to be %s %s'%(index,int(p*100),i,fix))
