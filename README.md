# forensic-watermarking with @ms4935, @15computer
DWT-DCT-CRT


## 목표

현재 널리 사용되는 워터마크는 육안으로 식별 가능하며 대부분 저작권 분쟁을 위한 곳에 한정돼 있다. 
또한, 이미지 기반의 다양한 창작물들에 대하여 불법 복제와 유통이 성행하는 현재, 기존의 워터마크 활용 방식을 확장하여 불법 복제와 유통 발생 시 이를 추적하는데 기여하는 포렌식 워터마킹 프로그램 제작을 목표로 한다. 
기존의 육안으로 식별 가능한 워터마크가 아닌 포렌식 워터마크 개념을 사용하여 이미지 대상 프로그램을 제작한다. 
이미지 이용자가 이미지를 사용할 때 해당 이용자의 정보를 이미지에 비가시적으로 삽입하고 추후에 삽입된 이미지에서 삽입된 정보를 추출 시 이용자의 정보를 확인할 수 있도록 한다.


## 개발 내용

해당 프로젝트는 삽입 데이터 생성, 이미지 대상 삽입, 이미지 대상 추출로 3가지 부분으로 나뉜다. 삽입에 유용한 모양을 가지며 표현 가능한 데이터의 양이 충분하고 약간의 손실에도 인식 가능한 QR코드를 생성부를 개발한다. 이어서, 이미지 사용 서비스의 질을 저하시키지 않기 위해 비가시성을 확보하며 불법 사용을 위한 삽입된 이미지에 대한 다양한 공격에 대항 강인성을 확보하는 워터마크 삽입부, 삽입된 이미지에서 워터마크 추출하는 워터마크 추출부를 개발한다.  

비가시성 확보를 위해 삽입할 이미지의 색공간을 RGB 형식에서 YUV 형식으로 변환하고 이에 DWT(Discrete wavelet transform), DCT(Discrete cosine transform)와 같은 주파수 영역 변환을 적용하고 JPEG 압축에 대한 강인성 확보를 위해 8x8 블록 단위 접근과 ICRT(Inverse-Chinese remainder theorem)을 통해 DC 값을 변경하여 삽입한다. 



### 개념 설계

사용자 정보를 QR code로 변환 생성한 후 이를 사용자가 이용하는 이미지에 삽입할 워터마크로 사용한다. 구현한 삽입과정을 거쳐 이미지에 워터마크를 삽입하고 이를 해당 이미지 사용자에게 전달한다. 추후에 워터마크가 삽입된 이미지에 대해 불법 복제, 유통이 의심되는 경우 구현한 추출과정을 통해 삽입된 워터마크를 추출한다. 추출된 워터마크인 QR code를 스캐너를 통해 데이터를 확인하고 이를 사용자 정보와 대조한다.


### 상세 설계
가로, 세로를 활용하여 숫자는 최대 7098자, 문자 최대 4296자를 기록할 수 있는 2차원적 구성을 띄는 QR code는 삽입하기 위한 데이터를 표현하기에 적합하다. 또한, 정사각형 형태를 가지기 때문에 삽입 시 한쪽 방향으로 치우치지 않으며 약간의 부분적 훼손에도 인식이 가능하다는 장점을 가진다. 사용자 식별 정보로 사용할 키 값을 32x32 크기의 QR code로 변환한 후 QR code의 각 위치의 픽셀 값에 따라 흰 부분의 경우 +3, 검은 부분의 경우 –3으로 설정하여 QR code의 삽입 값으로 사용한다. 

#### 워터마크 삽입 
YUV 형식을 이용하는 JPEG 압축에 대한 강인성을 위해 RGB 형식의 이미지를 YUV 형식으로 변환한다. 변환한 결과에서 JPEG 압축 시 데이터 손실이 적은 부분인 밝기를 나타내는 Y 채널을 대상으로 삽입과정을 진행한다. Y 채널을 대상으로 1-level DWT(Discrete wavelet transform)를 진행하여 원본 이미지의 1/4 크기인 LH 부대역을 획득한다.

LH 부대역을32개의 8x8 block으로 분할하고 각 block 대상으로 DCT(Discrete wavelet transform)를 진행하여 변환된 계수 값들을 획득한다. DCT를 통해 주파수 대역 별로 분해된 부대역을 주파수 영역으로 변환하고 저주파 대역에 에너지가 집중되는 에너지 집중 현상을 이용한다. 이렇게 획득한 변환 계수 값에서 이미지에 대한 데이터를 많이 가지고 있는 DC 값의 경우 JPEG 압축 시 손실되는 양이 적기 때문에 해당 위치에 워터마크를 삽입한다. 

 각각의 8x8 block의 (0,0)위치에 있는 DC값을 Z로 놓고 임의의 서로소 쌍{M,N}을 구한다. 모듈러 연산을 이용하여 Z값을 M으로 나누었을 때의 나머지를 R1, Z값을 N으로 나누었을 때의 나머지를 R2라 한다.
 기존 CRT에서는 D와 d를 이용하여 Z값을 변경하지만 본 프로젝트에서는 d대신 b를 사용하고 C를 이용하여 워터마크 삽입 강도를 결정짓는다. C 값을 조절하면 영상 품질을 저하시키지 않으면서도 적당한 삽입 강도를 찾아낼 수 있다. 

- 워터마크 비트 ‘1’
Z값에 대해 b>=(D+35)/2의 조건을 만족하는지 확인
조건 만족: DC값 = Z
조건 만족하지 않음: 
Z+8에 대해 b, D를 구한 후 조건 확인한다.
만약, 조건을 만족시키지 않으면, Z-8에 대해 조건 확인한다.

- 워터마크 비트 ‘0’
Z값에 대해 b<(D+35)/2의 조건을 만족하는지 확인
조건 만족: DC값 = Z
조건 만족하지 않음: 
Z+8에 대해 b, D를 구한 후 조건 확인한다.
만약, 조건을 만족시키지 않으면, Z-8에 대해 조건 확인한다.

이 과정을 조건식을 만족하는 Z값을 찾을 때까지 반복한다. Z값을 찾으면, DC값을 Z값으로 변경한다. 이어서 32개의 block에 대해 같은 과정을 진행한 후 IDCT(Inverse DCT)를 진행한다.

삽입된 HL, LH, HH 부대역을 가지는 DWT 결과에 대해 IDWT(Inverse DWT)를 진행하여 삽입된 Y 채널 이미지를 획득한다. 해당 Y 채널 이미지와 이전에 획득한 U, V 채널을 통합하여 YUV 형식의 이미지를 RGB 형식의 이미지로 변환하여 삽입과정을 완료한다.

