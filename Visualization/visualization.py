import 	traceback, sys
import 	os, time
import 	numpy as np
import 	qdarkgraystyle, qdarkstyle
import 	matplotlib.image as mpimg

from libs import settings

from subprocess import run
from subprocess import check_output
from subprocess import CalledProcessError

from 	matplotlib import pyplot as plt
from 	matplotlib.figure import Figure
from 	matplotlib.backends.qt_compat import QtWidgets
from 	matplotlib.backends.backend_qt5agg import (    FigureCanvas,
                                                    NavigationToolbar2QT as NavigationToolbar)
from 	PyQt5.QtGui 	import *
from 	PyQt5.QtCore 	import *
from 	PyQt5.QtWidgets import *


MATRIX_H = 544
MATRIX_W = 728

EXPOSURE_TIME_MAX = 85899300
EXPOSURE_TIME_MIN = 19



ON_label	= '<b><font color="green">LOOP IS ON</font></b>'
OFF_label	= '<b><font color="red">LOOP IS OFF</font></b>'


class MplCanvas(FigureCanvas):

    def __init__(self, parent=None, width=5, height=4, dpi=100):
        fig = Figure(figsize=(width, height), dpi=dpi)
        self.axes = fig.add_subplot(111)
        super(MplCanvas, self).__init__(fig)


class IMG_thread(QThread):
	'''
	Class to intialize image
	display using threads
	'''

	def __init__(self, main):
		QThread.__init__(self)
		self.main 		= main
		self.canvas 	= main.canvas

	def run(self):

		'''
		Display image
		'''
		print("Begin image display.")

		while self.main.loop_is_ON:

			#display image
			self.canvas.axes.cla()
			#make the marker optional : QBox
			if self.main.cog_marker_edit.isChecked():
				self.canvas.axes.scatter(float(self.main.cog_x), float(self.main.cog_y),c='C0', marker='+', s=1e4);
			self.canvas.axes.imshow(self.main.load_image(), cmap='gray')
			self.canvas.draw()

	def stop(self):

		#stop the acquisiton
		print("Stop image display.")
		
		#if acquisition is finished
		#quit the thread
		if self.wait():
			self.quit()

	def __del__(self):
		self.wait()

class RX_data_thread(QThread):
	'''
	Class to intialize data
	acquisition handling
	using threads
	'''
	def __init__(self, main):
		QThread.__init__(self)
		self.data_buffer= None
		self.main 		= main
	
	#Signals allow communication
	#between thread and the main
	#GUI process

	return_message = pyqtSignal(str)

	def run(self):
		'''
		Retreives the message from
		the image acquisition process
		'''

		print("Begin data RX.")

		print_flag = True
		while self.main.loop_is_ON:

			try:
				self.data_buffer = check_output(["./RX_TX_data/get_param"]).decode("utf-8")
				#send the acquired message to the main GUI process
				self.return_message.emit(self.data_buffer)

			except CalledProcessError:
				if print_flag:
					print("Message queue is empty. Please start the acquisition.")
					print_flag = False
				pass

	def stop(self):
		print("Stop data RX.")
		
		#if acquisition is finished
		#quit the thread
		if self.wait():
			self.quit()

	def __del__(self):
		self.wait()

class Main(QMainWindow):

	def __init__(self, parent = None):
		super(QMainWindow, self).__init__(parent)
		print("PYTHON VISUALIZATION/CONTROL GUI")
		#define variables
		self.loop_is_ON		= False
		self.thread_is_ON	= False
		self.data_buffer	= None
		self.cog_x 			= None
		self.cog_y			= None
		self.loop_state 	= None
		self.data_tx_rate 	= None
		self.vltg_avg_x 	= None
		self.vltg_avg_y 	= None
		self.expos_time 	= None

		#retrieve data from the acquisition process

		print_flag = True
		while True:

			try:
				self.data_buffer = check_output(["./RX_TX_data/get_param"]).decode("utf-8")
				break
			except CalledProcessError:
				if print_flag:
					print("Message queue is empty. Please start acquisition.")
				print_flag = False
			time.sleep(2)
				
		#decode received data
		self.decode_rx_data()
		
		#set up central widge
		central_widget = QFrame()
		self.hbox = QHBoxLayout(central_widget)
		self.setCentralWidget(central_widget)

		#set up figure canvas
		self.canvas = MplCanvas(self, width=5, height=4, dpi=100)
		self.hbox.addWidget(self.canvas)
		self.canvas.axes.imshow(self.load_image(), cmap='gray')
		
		#define buttons
		self.ON__button		= QPushButton('ON')
		self.OFF_button		= QPushButton('OFF')
		self.apply_button	= QPushButton('SEND SETTINGS')
		
		#define labels		
		self.loop_label 		= QLabel(OFF_label)
		self.vltg_avg_x_label 	= QLabel(self.vltg_avg_x + ' V')
		self.vltg_avg_y_label 	= QLabel(self.vltg_avg_y + ' V')
		self.expos_time_label	= QLabel(self.expos_time + ' μs')
		self.cog_x_label 		= QLabel(self.cog_x + ' pxl')
		self.cog_y_label 		= QLabel(self.cog_y + ' pxl')
		self.cog_label 			= QLabel('<b>COG </b>')
		self.camera_label 		= QLabel('<b>CAMERA </b>')
		self.tt_mirror_label 	= QLabel('<b>T-T MIRROR </b>')

		#define editable spin boxes
		self.frame_width_edit 	= QSpinBox()
		self.frame_height_edit 	= QSpinBox()
		self.x_offset_edit 		= QSpinBox()
		self.y_offset_edit 		= QSpinBox()
		self.data_tx_rate_edit	= QSpinBox()
		self.expos_time_edit	= QSpinBox()
		self.cog_x_ref_edit 	= QDoubleSpinBox()
		self.cog_y_ref_edit 	= QDoubleSpinBox()
		self.cog_gain_edit		= QDoubleSpinBox()

		#define check box
		self.cog_marker_edit	= QCheckBox()

		#Parametrize labels
		self.cog_label.setAlignment(Qt.AlignCenter)
		self.loop_label.setAlignment(Qt.AlignCenter)
		self.camera_label.setAlignment(Qt.AlignCenter)
		self.tt_mirror_label.setAlignment(Qt.AlignCenter)

		#Initialize Spin boxes
		self.x_offset_edit.setMaximum(MATRIX_W)
		self.y_offset_edit.setMaximum(MATRIX_H)
		#Initialize Spin boxes
		self.frame_width_edit.setValue(40)
		self.frame_height_edit.setValue(40)
		self.x_offset_edit.setValue(int(MATRIX_W/2))
		self.y_offset_edit.setValue(int(MATRIX_H/2))
		self.cog_x_ref_edit.setValue(0)
		self.cog_y_ref_edit.setValue(0)
		self.cog_gain_edit.setValue(0)
		self.expos_time_edit.setValue(int(self.expos_time))
		self.data_tx_rate_edit.setValue(int(self.data_tx_rate))

		self.frame_width_edit.setRange(0,MATRIX_W)
		self.frame_height_edit.setRange(0,MATRIX_H)
		self.x_offset_edit.setRange(0, MATRIX_W - self.frame_width_edit.value() )
		self.y_offset_edit.setRange(0, MATRIX_H - self.frame_height_edit.value())
		#TO DO: To verify x_ref, y_ref and gain values
		self.cog_x_ref_edit.setRange(0,MATRIX_W)
		self.cog_y_ref_edit.setRange(0,MATRIX_H)
		self.cog_gain_edit.setRange(0,10)
		self.expos_time_edit.setRange(EXPOSURE_TIME_MIN,EXPOSURE_TIME_MAX)
		self.data_tx_rate_edit.setRange(1, 65534)

		#Initialize check boxes
		self.cog_marker_edit.setCheckState(True)		
		self.cog_marker_edit.setTristate(False)

		#declare GUI events
		self.ON__button.clicked.connect(lambda:	self.update_acq_status(True))
		self.OFF_button.clicked.connect(lambda:	self.update_acq_status(False))
		self.apply_button.clicked.connect(self.tx_data)
		
		#TO DO: factor into a separate function
		self.frame_width_edit.valueChanged.connect(	lambda : self.x_offset_edit.setRange(0,MATRIX_W - self.frame_width_edit.value() ))
		self.frame_height_edit.valueChanged.connect(lambda : self.y_offset_edit.setRange(0,MATRIX_H - self.frame_height_edit.value()))

		#Define side menu widget that contains buttons & labels
		self.menu 		= QFrame()
		self.menu_grid 	= QGridLayout(self.menu)
		
		#Add to the side menu widget buttons & edits
		#addWidget(widget, row, col, row_extent, col_extent)
		#TO DO : add T-T ON/OFF button ?
		self.menu_grid.addWidget(self.loop_label, 0, 0, 1, 4)
		self.menu_grid.addWidget(self.ON__button, 1, 0, 1, 2)
		self.menu_grid.addWidget(self.OFF_button, 1, 2, 1, 2)
		self.menu_grid.addWidget(self.tt_mirror_label, 2, 0, 1, 4)
		self.menu_grid.addWidget(self.vltg_avg_x_label, 3, 1, 1, 1)
		self.menu_grid.addWidget(self.vltg_avg_y_label, 3, 3, 1, 1)
		self.menu_grid.addWidget(self.cog_label, 4, 0, 1, 4)
		self.menu_grid.addWidget(self.cog_x_label, 5, 1, 1, 1)
		self.menu_grid.addWidget(self.cog_y_label, 5, 3, 1, 1)
		self.menu_grid.addWidget(self.cog_x_ref_edit, 6, 1, 1, 1)
		self.menu_grid.addWidget(self.cog_y_ref_edit, 6, 3, 1, 1)
		self.menu_grid.addWidget(self.cog_gain_edit, 7, 1, 1, 1)
		self.menu_grid.addWidget(self.cog_marker_edit, 7, 3, 1, 1)
		self.menu_grid.addWidget(self.camera_label, 8, 0, 1, 4)
		self.menu_grid.addWidget(self.x_offset_edit, 10, 1, 1, 1)
		self.menu_grid.addWidget(self.y_offset_edit, 10, 3, 1, 1)
		self.menu_grid.addWidget(self.frame_width_edit ,9, 1, 1, 1)
		self.menu_grid.addWidget(self.frame_height_edit,9, 3, 1, 1)
		self.menu_grid.addWidget(self.data_tx_rate_edit, 12, 2, 1, 1)
		self.menu_grid.addWidget(self.expos_time_edit, 11, 2, 1, 2)
		self.menu_grid.addWidget(self.apply_button, 13, 0, 1, 4)
 
		#Add to the side menu widget description labels
		self.menu_grid.addWidget(QLabel('Voltage average x'), 3, 0, 1, 1)
		self.menu_grid.addWidget(QLabel('Voltage average y'), 3, 2, 1, 1)
		self.menu_grid.addWidget(QLabel('Cog X:'), 5, 0, 1, 1)
		self.menu_grid.addWidget(QLabel('Cog Y:'), 5, 2, 1, 1)
		self.menu_grid.addWidget(QLabel('Set Cog X reference:'), 6, 0, 1, 1)
		self.menu_grid.addWidget(QLabel('Set Cog Y reference:'), 6, 2, 1, 1)
		self.menu_grid.addWidget(QLabel('Set COG gain:'), 7, 0, 1, 1)
		self.menu_grid.addWidget(QLabel('Set COG marker:'), 7, 2, 1, 1)
		self.menu_grid.addWidget(QLabel('Set frame X offset (pxl) :'), 10, 0, 1, 1)
		self.menu_grid.addWidget(QLabel('Set frame Y offset (pxl) :'), 10, 2, 1, 1)
		self.menu_grid.addWidget(QLabel('Set frame width  (pxl):'),  9, 0, 1, 1)
		self.menu_grid.addWidget(QLabel('Set frame height (pxl):'), 9, 2, 1, 1)
		self.menu_grid.addWidget(QLabel('Set camera exposure time (µs):'), 11, 0, 1, 2)
		self.menu_grid.addWidget(QLabel('Set image and data TX rate to every:'), 12, 0, 1, 2)
		self.menu_grid.addWidget(QLabel(' iterations'), 12, 3, 1, 1)

		self.menu_grid.setRowStretch(14, 1)
		
		#Add side menu widget to the main window
		self.hbox.addWidget(self.menu)
		
		self.setWindowTitle('TT mirror loop visualization')

	def update_acq_status(self, loop_is_ON):
		'''
		Updates loop labels, handles message acquisiton 
		and updates UI by running separate threads
		'''
		self.loop_is_ON = loop_is_ON
		
		#if ON button is pressed and thread is off
		if loop_is_ON and not self.thread_is_ON:

			self.loop_label.setText(ON_label)
			self.thread_is_ON = True

			#initialize data RX thread
			self.RX_thread = RX_data_thread(self)
			self.RX_thread.start()
			#return message from the thread to the main GUI
			self.RX_thread.return_message.connect(self.update_UI)

			#initialize IMG display thread
			self.Img_thread = IMG_thread(self)
			self.Img_thread.start()

		elif not loop_is_ON and self.thread_is_ON:

			self.loop_label.setText(OFF_label)

			#stop threads
			self.thread_is_ON = False
			self.RX_thread.stop()
			self.Img_thread.stop()

	def update_UI(self, data_buffer):

		self.data_buffer = data_buffer

		#decode data
		self.decode_rx_data()

		#update GUI QLabels
		self.vltg_avg_x_label.setText(self.vltg_avg_x + ' V' )
		self.vltg_avg_y_label.setText(self.vltg_avg_y + ' V' )
		self.cog_x_label.setText(self.cog_x + ' pxl')
		self.cog_y_label.setText(self.cog_y + ' pxl')

	def tx_data(self):

		arg1	= str(self.cog_x_ref_edit.value())
		arg2	= str(self.cog_y_ref_edit.value())
		arg3	= str(self.cog_gain_edit.value())
		arg4	= str(self.frame_width_edit.value())
		arg5	= str(self.frame_height_edit.value())
		arg6	= str(self.x_offset_edit.value())
		arg7	= str(self.y_offset_edit.value())
		arg8	= str(self.expos_time_edit.value())
		arg9	= str(self.data_tx_rate_edit.value())
		arg10 	= str(int(self.loop_is_ON))

		
		try:
			run(["RX_TX_data/./set_param",arg1,
									arg2,
									arg3,
									arg4,
									arg5,
									arg6,
									arg7,
									arg8,
									arg9,
									arg10],
									check = True)
		except CalledProcessError:
			#TO DO: Pop-up window
			print("Data is not transmitted. Retry.") 
	
	
	#Faster load image algorihm - with pixelation
	def load_image(self):

		image = []
		lines = self.get_loop_param('img').split('\n')
		for line in lines:
			if line != '':
				image.append(line.split(','))
		np_image = np.array(image, dtype= float)
		return np_image

	#Slower load image algorithm - no pixelation
	'''
	def load_image(self):
		result = self.get_loop_param('img')

		image = []
		for line in result.split('\n'):

			values = line.split(',')

			values_conv = []
			for value in values:

				if value != '':

					float_v = float(value)
					values_conv.append([float_v, float_v, float_v])

			if values_conv != []:

				image.append(values_conv)
		
		return np.array(image)
	'''

	def get_loop_param(self, param_name):

		param_name_to_index = {'loop_state': 0,
							'cog_x'	: 1,
							'cog_y'	: 2,
							'voltage_avg_x'	: 3,
							'voltage_avg_y'	: 4,
							'exposure_time'	: 5,
							'data_tx_rate':6,
							'img': 7}

		result = self.data_buffer.split('_')[param_name_to_index[param_name]]
		
		return result

	def decode_rx_data(self):

		self.cog_x 			= self.get_loop_param('cog_x')
		self.cog_y			= self.get_loop_param('cog_y')
		self.loop_state		= self.get_loop_param('loop_state')
		self.data_tx_rate 	= self.get_loop_param('data_tx_rate')
		self.expos_time 	= self.get_loop_param('exposure_time')
		self.vltg_avg_x 	= self.get_loop_param('voltage_avg_x')
		self.vltg_avg_y 	= self.get_loop_param('voltage_avg_y')


if __name__ == "__main__":
	if not QApplication.instance():
		app = QApplication(sys.argv)
	else:
		app = QApplication.instance()

	file = open('libs/style.css', mode='r')
	css_content = file.read()
	file.close()
	app.setStyleSheet(qdarkstyle.load_stylesheet() + css_content)

	win = Main()
	win.resize(settings.window_width, settings.window_height)
	win.show()

	sys.exit(app.exec())
