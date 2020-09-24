# forensic-watermarking with @ms4935, @15computer
DWT-DCT-CRT


### 목표

현재 널리 사용되는 워터마크는 육안으로 식별 가능하며 대부분 저작권 분쟁을 위한 곳에 한정돼 있다. 
또한, 이미지 기반의 다양한 창작물들에 대하여 불법 복제와 유통이 성행하는 현재, 기존의 워터마크 활용 방식을 확장하여 불법 복제와 유통 발생 시 이를 추적하는데 기여하는 포렌식 워터마킹 프로그램 제작을 목표로 한다. 
기존의 육안으로 식별 가능한 워터마크가 아닌 포렌식 워터마크 개념을 사용하여 이미지 대상 프로그램을 제작한다. 
이미지 이용자가 이미지를 사용할 때 해당 이용자의 정보를 이미지에 비가시적으로 삽입하고 추후에 삽입된 이미지에서 삽입된 정보를 추출 시 이용자의 정보를 확인할 수 있도록 한다.


### 개발 내용

해당 프로젝트는 삽입 데이터 생성, 이미지 대상 삽입, 이미지 대상 추출로 3가지 부분으로 나뉜다. 삽입에 유용한 모양을 가지며 표현 가능한 데이터의 양이 충분하고 약간의 손실에도 인식 가능한 QR코드를 생성부를 개발한다. 이어서, 이미지 사용 서비스의 질을 저하시키지 않기 위해 비가시성을 확보하며 불법 사용을 위한 삽입된 이미지에 대한 다양한 공격에 대항 강인성을 확보하는 워터마크 삽입부, 삽입된 이미지에서 워터마크 추출하는 워터마크 추출부를 개발한다.  

비가시성 확보를 위해 삽입할 이미지의 색공간을 RGB 형식에서 YUV 형식으로 변환하고 이에 DWT(Discrete wavelet transform), DCT(Discrete cosine transform)와 같은 주파수 영역 변환을 적용하고 JPEG 압축에 대한 강인성 확보를 위해 8x8 블록 단위 접근과 ICRT(Inverse-Chinese remainder theorem)을 통해 DC 값을 변경하여 삽입한다. 