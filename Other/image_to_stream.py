import matplotlib.pyplot as plt
import matplotlib.image as mpimg

# run the script with python3 image_to_stream.py
# /!\ FROM THE git_loop_it FOLDER

img = mpimg.imread('PSF_images/centered_with_col.png')

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