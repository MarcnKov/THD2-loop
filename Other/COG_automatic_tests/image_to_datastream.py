import sys
import matplotlib.pyplot as plt
import matplotlib.image as mpimg

splitted_arg = sys.argv[1].split(';')
img_path = splitted_arg[0]

f = open("expected.txt", "w")
f.write(splitted_arg[1])
f.close()

img = mpimg.imread(img_path)

output = '{' # for C syntax
for line in img:
	if output != '{':
		output += ','
	output += '{'
	first_el = True
	for pixel in line:
		if not first_el:
			output += ', '
		else:
			first_el = False
		output += str(pixel[0]) # red extraced, black/white images have the same RGB values
	output += '}'
output += '}'

print(output)

#imgplot = plt.imshow(img)
#plt.show()