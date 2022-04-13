import numpy
import matplotlib.pyplot as plt
from sklearn.metrics import r2_score

# generate random
numpy.random.seed(2)

x = numpy.random.normal(3, 1, 100)
y = numpy.random.normal(150, 40, 100) / x

train_x = x[:80]
train_y = y[:80]

test_x = x[80:]
test_y = y[80:]

# create a polynomial regression model of the data
# (similar to line of best fit)
mymodel = numpy.poly1d(numpy.polyfit(train_x, train_y, 5))

# configure line
myline = numpy.linspace(0, 6, 100)

"""**************************************************************
R2 or R-squared
It measures the relationship between the x axis and the y axis,
and the value ranges from 0 to 1, where 0 means no relationship,
and 1 means totally related.
**************************************************************"""
train_r2 = r2_score(train_y, mymodel(train_x))
test_r2 = r2_score(test_y, mymodel(test_x))

print(f"rd score: {train_r2}, the trained model is {train_r2 * 100}% accurate")
print(f"rd score: {test_r2}, the test model is {test_r2 * 100}% accurate")

"""**************************************************************
After establishing the model, we can use the model to 
predict the behaviour by plugging a value into the model
**************************************************************"""
print(f"prediction mymodel(5): {mymodel(5)}")


# plot x,y data
plt.scatter(train_x, train_y)

# plot line
plt.plot(myline, mymodel(myline))
plt.show()


