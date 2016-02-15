import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as patches

stuff = np.genfromtxt('positions.txt', dtype = None, names = "dt, id, x, y, z, vx, vy, vz")
number = max(stuff['id'])+1
timesteps = stuff.size/number

data = np.reshape(stuff, (timesteps,number))
x = data['x']
y = data['y']
z = data['z']

for i in range (0, timesteps-1) :
	    fig = plt.figure(figsize=(8,8))
    	ax = plt.subplot(111,aspect='equal')
    	ax.set_ylim(-40000, 40000)
   	  ax.set_xlim(-80000,300000)
	    
	    #for j in range(0, number-1):
        #	circ = patches.Circle((y[i,j], x[i,j]), 130, facecolor='darkgray', edgecolor='black')
        #	ax.add_patch(circ)

	    plt.plot(y[i,:], x[i,:], 'kp')
	    #plt.show()

	    plt.savefig("%d.png" % (i))
	    plt.close()

# Plot single particle's trajectory
# plt.plot(y[np.where(id == 0)], x[np.where(id == 0)])
