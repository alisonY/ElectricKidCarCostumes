//不带遥控功能的儿童电动童车改线性油门
//使用Arduino模拟端口接收霍尔油门踏板信号
//再由数字端口输出PWM信号控制有刷电调驱动双路直流电机
//ardunio nano,火凤凰480A,3S锂电池

int escPin = 9; //舵机接口
int throPadPin = A0;//踏板电位器模拟输入口
int throAdjPin = A2;//油门调整旋钮电位器输入口
int switchPin0 = 6; //前后开关
int switchPin1 = 4;//前后开关

int pulsewidth;//定义脉宽变量

int finalVal;
int myangle1;

int throPadInput;//踏板最终输出
int throAdjInput;//油门调整输入
int output;//最终输出值

int throPadMin = 183;//踏板最小
int throPadMax = 863;//踏板最大

int expectMax = 390;//最大油门时踏板电位器值

int forwardOutVal = 2480;
int reverseOutVal = 500;

int offsetMin;//期望输入大小偏移
int offsetMax;//实际输入大小偏移
float offset;
float adjustVal;//电位器调整量
/***********************/
int status = 0;//-1后退 0空 1前进
/***********************/
int inited = 1;

//下面是servopulse函数部分(此函数意思:也就是說每次都是0.5ms高電平 1.98ms低電平 然後再0.52ms低電平 17ms延時也是低電平)
void servopulse(int escPin, int finalVal, int outVal) {
  myangle1 = map(finalVal, throPadMin, throPadMax, 1490, outVal);//370
  digitalWrite(escPin, HIGH); //将舵机接口电平至高
  delayMicroseconds(myangle1);//延时脉宽值的微秒数
  digitalWrite(escPin, LOW); //将舵机接口电平至低
}


//开关状态
void getSwitchStat() {
  int val0 = digitalRead(switchPin0);
  int finalVal = digitalRead(switchPin1);
  if (val0 != 0 && finalVal == 0) {
    //Serial.println("REVERSE");
    status = -1;
  }
  if (val0 == 0 && finalVal != 0) {
    //Serial.println("FORWARD");
    status = 1;
  }
  if (val0 == 1 && finalVal == 1) {
    status = 0;
    //Serial.println("NEUTRAL");
  }

}

//servopulse函数部分到此结束
void setup()
{
  pinMode(switchPin0, INPUT_PULLUP);
  pinMode(switchPin1, INPUT_PULLUP);
  pinMode(escPin, OUTPUT); //设定舵机接口为输出接口
  Serial.begin(9600);
  Serial.println("hello world,tel:416800698" ) ;

  offsetMin = expectMax - throPadMin;
  offsetMax = throPadMax - throPadMin;
  offset = (float)offsetMin / offsetMax;

}

void loop()
{
  getSwitchStat();//获取开关状态
  throPadInput = analogRead(throPadPin);//读取油门踏板信号
  throAdjInput = analogRead(throAdjPin);//读取油门调整电位器信号
  
  adjustVal = (float)throAdjInput/1023;//百分化
  Serial.print("throPadInput = ");
  Serial.print(throPadInput, DEC);
  Serial.print(",throAdjInput = ");
  Serial.print(adjustVal, DEC);
  if(throPadInput>throPadMin){
    throPadInput = adjustVal * ( expectMax - ((throPadMax - throPadInput) * offset));
  }else{
    throPadInput = expectMax - ((throPadMax - throPadInput) * offset);
    }
  Serial.print(",finalVal = ");
  Serial.print(finalVal);
  Serial.print(",switch =  ");
  Serial.println(status);

  if (status == 1 && inited == 1) {
    servopulse(escPin, throPadInput, forwardOutVal); //引用脉冲函数
  }
  if (status == -1 && inited == 1) {
    servopulse(escPin, throPadInput+5, reverseOutVal); //引用脉冲函数
  }
  if (status == 0) {
    //无操作
  }
  
}
