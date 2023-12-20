import sys
import socket
from PyQt5.QtWidgets import *
from PyQt5 import uic
import matplotlib
import pandas as pd
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
import numpy as np

form_class = uic.loadUiType("map_new.ui")[0]

City = {'서울특별시':1, '경기도':2, '경상북도':3, '경상북도':4, "전라남도":5, '전라북도':6, "충청남도":7, '충청북도':8, '제주특별자치도':9, '강원도':10}

Item = {'쌀': 111, '찹쌀':112, '콩':141, '팥':142, '녹두':143, '메밀':144, '고구마':151, '감자':152, '배추':211, '양배추':212,\
        '시금치':213, '상추':214, '얼갈이배추':215, '갓':216, '수박':221, '참외':222, '오이':223, '호박':224, '토마토':225, \
        '딸기':226, '무':231, '당근':232, '열무':233, '건고추':241, '풋고추':242, '붉은고추':243, '마늘':244, '양파':245,\
        '파':246, '생강':247, '고춧가루':248, '미나리':252, '깻잎':253, '피망(청)':255, '파프리카':256, '멜론':257, '참깨':312,\
        '들깨':313, '땅콩':314, '느타리버섯':315, '팽이버섯':316, '새송이버섯':317, '호두':318, '아몬드':319, '사과':411, '배':412,\
        '복숭아':413, '포도':414, '감귤':415, '단감':416, '바나나':418, '참다래':419, '파인애플':420, '오렌지':421, '방울토마토':422,\
        '레몬':424, '체리':425, '건포도':426, '건블루베리':427, '망고':428}


class WindowClass(QMainWindow, form_class) :
    def __init__(self):
        super().__init__()
        self.HOST = '10.10.20.111'  # 서버의 IP 주소
        self.PORT = 9105  # 서버의 포트 번호
        # 서버에 연결
        self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.client_socket.connect((self.HOST, self.PORT))
        self.setupUi(self)
        # 리스트
        self.message = []
        self.total = []
        self.total1 =[]
        self.save = []
        # 실행시 첫 화면 지정
        self.stackedWidget.setCurrentIndex(0)
        # 첫 화면 넘어가기 버튼
        self.pushButton.clicked.connect(self.btn_main_exe)
        # 콤보박스 비활성화
        self.comboBox_2.setDisabled(True)
        # 푸시버튼 비활성화
        self.pushButton1.setDisabled(True)
        self.pushButton2.setDisabled(True)
        self.pushButton3.setDisabled(True)
        self.pushButton4.setDisabled(True)
        self.pushButton5.setDisabled(True)
        self.pushButton6.setDisabled(True)
        self.pushButton7.setDisabled(True)
        self.pushButton8.setDisabled(True)
        self.pushButton9.setDisabled(True)
        self.pushButton10.setDisabled(True)
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
        # 경상북도 지역 정보
        self.pushButton6.clicked.connect(self.area_clicked6)
        # 전라북도 지역 정보
        self.pushButton7.clicked.connect(self.area_clicked7)
        # 경상남도 지역 정보
        self.pushButton8.clicked.connect(self.area_clicked8)
        # 전라남도 지역 정보
        self.pushButton9.clicked.connect(self.area_clicked9)
        # 제주특별자치도 지역 정보
        self.pushButton10.clicked.connect(self.area_clicked10)





    def btn_main_exe(self): # 메인화면 -> 지역화면
        self.stackedWidget.setCurrentIndex(1)
    def comboBoxClicked0(self):  # 1 콤보박스 연도
        self.message = []
        a=self.comboBox.currentText()
        if a == '-':
            self.pushButton1.setDisabled(True)
            self.pushButton2.setDisabled(True)
            self.pushButton3.setDisabled(True)
            self.pushButton4.setDisabled(True)
            self.pushButton5.setDisabled(True)
            self.pushButton6.setDisabled(True)
            self.pushButton7.setDisabled(True)
            self.pushButton8.setDisabled(True)
            self.pushButton9.setDisabled(True)
            self.pushButton10.setDisabled(True)
            return
        print(self.total)
        self.pushButton1.setEnabled(True)
        self.pushButton2.setEnabled(True)
        self.pushButton3.setEnabled(True)
        self.pushButton4.setEnabled(True)
        self.pushButton5.setEnabled(True)
        self.pushButton6.setEnabled(True)
        self.pushButton7.setEnabled(True)
        self.pushButton8.setEnabled(True)
        self.pushButton9.setEnabled(True)
        self.pushButton10.setEnabled(True)
        self.message.append(a)
    def area_clicked1(self):  # 서울 지역 정보
        b=self.pushButton1.text()
        if len(self.message) == 2:
            self.message.pop()
        self.message.append(b)
        self.total= City[self.message[1]]
        self.comboBox_2.setEnabled(True)
    def area_clicked2(self):  # 경기도 지역 정보
        b=self.pushButton2.text()
        if len(self.message) == 2:
            self.message.pop()
        self.message.append(b)
        self.total = City[self.message[1]]
        self.comboBox_2.setEnabled(True)
    def area_clicked3(self):  # 강원도 지역 정보
        b=self.pushButton3.text()
        if len(self.message) == 2:
            self.message.pop()
        self.message.append(b)
        self.total = City[self.message[1]]
        self.comboBox_2.setEnabled(True)
    def area_clicked4(self):  # 충청북도 지역 정보
        b=self.pushButton4.text()
        if len(self.message) == 2:
            self.message.pop()
        self.message.append(b)
        self.total = City[self.message[1]]
        self.comboBox_2.setEnabled(True)
    def area_clicked5(self):  # 충청남도 지역 정보
        b=self.pushButton5.text()
        if len(self.message) == 2:
            self.message.pop()
        self.message.append(b)
        self.total = City[self.message[1]]
        self.comboBox_2.setEnabled(True)
    def area_clicked6(self):  # 경상북도 지역 정보
        b=self.pushButton6.text()
        if len(self.message) == 2:
            self.message.pop()
        self.message.append(b)
        self.total = City[self.message[1]]
        self.comboBox_2.setEnabled(True)
    def area_clicked7(self):  # 전라북도 지역 정보
        b=self.pushButton7.text()
        if len(self.message) == 2:
            self.message.pop()
        self.message.append(b)
        self.total = City[self.message[1]]
        self.comboBox_2.setEnabled(True)
    def area_clicked8(self):  # 경상남도 지역 정보
        b=self.pushButton8.text()
        if len(self.message) == 2:
            self.message.pop()
        self.message.append(b)
        self.total = City[self.message[1]]
        self.comboBox_2.setEnabled(True)
    def area_clicked9(self):  # 전라남도 지역 정보
        b=self.pushButton9.text()
        if len(self.message) == 2:
            self.message.pop()
        self.message.append(b)
        self.total = City[self.message[1]]
        self.comboBox_2.setEnabled(True)
    def area_clicked10(self):  # 제주특별자치도 지역 정보
        b=self.pushButton10.text()
        if len(self.message) == 2:
            self.message.pop()
        self.message.append(b)
        self.total = City[self.message[1]]
        self.comboBox_2.setEnabled(True)

    def comboBox_1_Clicked(self):  # 2 콤보박스 품목
        c=self.comboBox_2.currentText()
        if c=='-':
            self.pushButton1.setDisabled(True)
            self.pushButton2.setDisabled(True)
            self.pushButton3.setDisabled(True)
            self.pushButton4.setDisabled(True)
            self.pushButton5.setDisabled(True)
            self.pushButton6.setDisabled(True)
            self.pushButton7.setDisabled(True)
            self.pushButton8.setDisabled(True)
            self.pushButton9.setDisabled(True)
            self.pushButton10.setDisabled(True)
            return
        self.comboBox_2.setDisabled(True)

        if len(self.message) == 3:
            self.message.pop()
        self.message.append(c)
        self.total = City[self.message[1]]
        self.total1 = Item[self.message[2]]
        self.save = "{},{}".format(self.total,self.total1)
        self.send=self.message[0]+ ',' + self.save

        # #데이터 송신
        # self.client_socket.sendall(self.send.encode('utf-8'))
        # self.client_socket.recv(1024)
        # 여기까지 확인완료
        #
        # # 서버로부터 데이터 수신
        # Data = self.client_socket.recv(1000000)
        # with open('file.csv','wb') as f:
        #     while Data:
        #         f.write(Data)
        #         # self.client_socket.send(data.encode(encoding='utf-8'))
        #         Data = self.client_socket.recv(10000)

        # 데이터 불러오기
        data = pd.read_csv('file.csv')


        # 서브플롯 생성 (1행 2열)
        fig, axs = plt.subplots(1, 2, figsize=(15, 6))

        # 첫 번째 서브플롯: 곡선 형태의 선 그래프
        axs[0].plot(data['규격'], label='price')
        axs[0].set_title('Price Change')
        axs[0].set_ylabel('Price')
        axs[0].legend()
        axs[0].grid(True)

        axs[0].set_xticks([])
        axs[0].set_ylim(0, 100000)

        # 두 번째 서브플롯: 규격별 그래프
        specifications = data['규격'].unique()
        for spec in specifications:
            spec_data = data[data['규격'] == spec]
            axs[1].plot(spec_data['조사가격'], label=f'price ({spec})')

        axs[1].set_title('Price Change')
        axs[1].set_ylabel('Price')
        axs[1].legend()
        axs[1].grid(True)

        plt.xticks([])
        plt.ylim(0, 100000)
        plt.show()

        self.comboBox.setCurrentIndex(0)
        self.comboBox_2.setCurrentIndex(0)

        self.closesock()
    def closesock(self):
        self.client_socket.close()

if __name__ == "__main__" :
    app = QApplication(sys.argv)
    myWindow = WindowClass()
    myWindow.show()
    app.exec_()
