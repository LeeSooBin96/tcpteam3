import sys
from PyQt5.QtWidgets import *
from PyQt5 import uic
import matplotlib
import pandas as pd
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
import numpy as np

form_class = uic.loadUiType("map.ui")[0]

class WindowClass(QMainWindow, form_class) :

    def __init__(self):
        super().__init__()
        self.setupUi(self)
        # 리스트
        self.message = []
        # 실행시 첫 화면 지정
        self.stackedWidget.setCurrentIndex(0)
        # 첫 화면 넘어가기 버튼
        self.pushButton.clicked.connect(self.btn_main_exe)
        # 서울 버튼
        self.pushButton1.clicked.connect(self.btn_area_exe)
        # 경기도
        self.pushButton2.clicked.connect(self.btn_area_exe)
        # 강원도
        self.pushButton3.clicked.connect(self.btn_area_exe)
        # 충청북도
        self.pushButton4.clicked.connect(self.btn_area_exe)
        # 충청남도
        self.pushButton5.clicked.connect(self.btn_area_exe)
        # 대전광역시
        self.pushButton6.clicked.connect(self.btn_area_exe)
        # 경상북도
        self.pushButton7.clicked.connect(self.btn_area_exe)
        # 전라북도
        self.pushButton8.clicked.connect(self.btn_area_exe)
        # 경상남도
        self.pushButton9.clicked.connect(self.btn_area_exe)
        # 광주광역시
        self.pushButton10.clicked.connect(self.btn_area_exe)
        # 전라남도
        self.pushButton11.clicked.connect(self.btn_area_exe)
        # 제주특별자치도
        self.pushButton12.clicked.connect(self.btn_area_exe)
        # 뒤로가기
        self.pushButton13.clicked.connect(self.btn_back_exe)
        # 1 화면 연도 콤보박스 클릭
        self.comboBox.currentIndexChanged.connect(self.comboBoxClicked0)
        # 2 화면 품목 콤보박스 클릭
        self.comboBox_2.currentIndexChanged.connect(self.comboBox_1_Clicked)
        # 서울 지역 정보
        self.pushButton1.clicked.connect(self.area_clicked1)
        # 경기도 지역 정보
        self.pushButton2.clicked.connect(self.area_clicked2)
        # 강원도 지역 정보
        self.pushButton3.clicked.connect(self.area_clicked3)
        # 충청북도 지역 정보
        self.pushButton4.clicked.connect(self.area_clicked4)
        # 충청남도 지역 정보
        self.pushButton5.clicked.connect(self.area_clicked5)
        # 대전광역시 지역 정보
        self.pushButton6.clicked.connect(self.area_clicked6)
        # 경상북도 지역 정보
        self.pushButton7.clicked.connect(self.area_clicked7)
        # 전라북도 지역 정보
        self.pushButton8.clicked.connect(self.area_clicked8)
        # 경상남도 지역 정보
        self.pushButton9.clicked.connect(self.area_clicked9)
        # 광주광역시 지역 정보
        self.pushButton10.clicked.connect(self.area_clicked10)
        # 전라남도 지역 정보
        self.pushButton11.clicked.connect(self.area_clicked11)
        # 제주특별자치도 지역 정보
        self.pushButton12.clicked.connect(self.area_clicked12)
        # 클릭 횟수를 저장하는 변수
        self.combobox_count = 0

        self.comboBox.activated.connect(self.combobox_count)



        #for draw graph
        # self.fig = plt.Figure()
        # self.canvas = FigureCanvas(self.fig)
        #
        # self.graph_verticalLayout.addWidget(self.canvas)
        #
        # df = pd.read_csv('/home/iot/PycharmProjects/pythonProject/.venv/2013_01_.csv')
        # filt = df['품목명'].str.startswith('열무')
        #
        #
        #
        # x = np.arange(0,100000,10000)
        # y = np.sin(x)
        # ax = self.fig.add_subplot(111)
        # ax.plot(x, y, label="sin")
        #
        #
        # ax.set_title("my sin graph")
        # ax.legend()
        # self.canvas.draw()



    def btn_main_exe(self): # 메인화면 -> 지역화면
        self.stackedWidget.setCurrentIndex(1)
    def btn_area_exe(self): # 지역화면 -> 그래프화면
        self.stackedWidget.setCurrentIndex(2)
    def btn_back_exe(self): # 그래프화면 -> 지역화면
        self.stackedWidget.setCurrentIndex(1)
        self.message.clear()
    def comboBoxClicked0(self):  # 1 콤보박스 연도
        a=self.comboBox.currentText()
        # self.message.append(a)

        if self.combobox_count%2 == 0:
            self.message.clear()
        else:
            self.message.append(a)

        print(self.message)
    def area_clicked1(self):  # 서울 지역 정보
        b=self.pushButton1.text()
        self.message.append(b)
        print(self.message)
    def area_clicked2(self):  # 경기도 지역 정보
        self.pushButton2.text()
    def area_clicked3(self):  # 강원도 지역 정보
        self.pushButton3.text()
    def area_clicked4(self):  # 충청북도 지역 정보
        self.pushButton4.text()
    def area_clicked5(self):  # 충청남도 지역 정보
        self.pushButton5.text()
    def area_clicked6(self):  # 대전광역시 지역 정보
        self.pushButton6.text()
    def area_clicked7(self):  # 경상북도 지역 정보
        self.pushButton7.text()
    def area_clicked8(self):  # 전라북도 지역 정보
        self.pushButton8.text()
    def area_clicked9(self):  # 경상남도 지역 정보
        self.pushButton9.text()
    def area_clicked10(self):  # 광주광역시 지역 정보
        self.pushButton10.text()
    def area_clicked11(self):  # 전라남도 지역 정보
        self.pushButton11.text()
    def area_clicked12(self):  # 제주특별자치도 지역 정보
        self.pushButton12.text()
    def comboBox_1_Clicked(self):  # 2 콤보박스 품목
        c=self.comboBox_2.currentText()
        self.message.append(c)
        print(self.message)
    def combobox_count(self):
        self.combobox_count+=1




if __name__ == "__main__" :
    app = QApplication(sys.argv)
    myWindow = WindowClass()
    myWindow.show()
    app.exec_()