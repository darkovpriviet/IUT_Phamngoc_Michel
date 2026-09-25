using System.IO.Ports;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using ExtendedSerialPort_NS;
using System.Windows.Threading;
using System.Net.NetworkInformation;
//using KeyboardHook_NS;
using System.Windows.Media.Animation;
using WpfAsservissementDisplay_NS;
using SciChart.Data.Model;
using static SciChart.Drawing.Utility.PointUtil;
using System.Security.Cryptography.Xml;
using System.Windows.Ink;
using SciChart.Charting.Common.Databinding;
using System;

//kp==7 et Kp==140




namespace RobotInterface
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    /// 

    public partial class MainWindow : Window
    {
    
     
        int step = 30;
        private Ellipse waypointPoint;
        bool toogle, b;
        byte i;
        bool autoControlActivated;
        string receivedText;
        ExtendedSerialPort serialPort1;
        DispatcherTimer timerAffichage;
        Robot robot = new Robot();
        private RotateTransform RotationGhost = new RotateTransform();
        private TranslateTransform TranslationGhost = new TranslateTransform();

        private double lastAngle = 0;





        public MainWindow()
        {
          
            timerAffichage = new DispatcherTimer();
            timerAffichage.Interval = new TimeSpan(0, 0, 0, 0, 10);
            timerAffichage.Tick += TimerAffichage_Tick;
            timerAffichage.Start();
            InitializeComponent();
            serialPort1 = new ExtendedSerialPort("COM3", 115200, Parity.None, 8, StopBits.One);
            serialPort1.DataReceived += SerialPort1_DataReceived;
            serialPort1.Open();
           //var _globalKeyboardHook = new GlobalKeyboardHook();
           // _globalKeyboardHook.KeyPressed += _globalKeyboardHook_KeyPressed;
            this.Loaded += MainWindow_Loaded;
    









        }



        private void MainWindow_Loaded(object sender, RoutedEventArgs e)
        {
            for (double x = 0; x <= myGrid.ActualWidth; x += step)
            {
                myGrid.Children.Add(new System.Windows.Shapes.Line
                {
                    X1 = x,
                    Y1 = 0,
                    X2 = x,
                    Y2 = myGrid.ActualHeight,
                    Stroke = Brushes.LightGray,
                    StrokeThickness = 1
                });
            }

            for (double y = 0; y <= myGrid.ActualHeight; y += step)
            {
                myGrid.Children.Add(new System.Windows.Shapes.Line
                {
                    X1 = 0,
                    Y1 = y,
                    X2 = myGrid.ActualWidth,
                    Y2 = y,
                    Stroke = Brushes.LightGray,
                    StrokeThickness = 1
                });
            }
            Canvas.SetLeft(PositionGhost, myGrid.ActualWidth / 2);
            Canvas.SetTop(PositionGhost, myGrid.ActualHeight / 2);

            TransformGroup group = new TransformGroup();

            group.Children.Add(RotationGhost);
            group.Children.Add(TranslationGhost);

            PositionGhost.RenderTransform = group;

        }





        private void SizeGrid(object sender, EventArgs e)
        {
           

        }

        private Ellipse waypoint = new Ellipse
        {
            Width = 12,
            Height = 12,
            Fill = Brushes.Red
        };



        public void TimerAffichage_Tick(object sender, EventArgs e)
        {

            /*  if (robot.receivedText != "")
                  //TextBoxréception.Text += receivedText;
                  TextBoxréception.Text += robot.receivedText;

                  robot.receivedText = "";
              robot.receivedText = robot.byteListReceived.ToString();
            */
            //TextBoxréception.Text= robot.receivedText;
            while (robot.byteListReceived.Count > 0)
            {
                byte Received = robot.byteListReceived.Dequeue();

                //TextBoxréception.Text += "0x" + Received.ToString("X2") + " "; //X2 c'est pour convertir en Hexadécimal 
                DecodeMessage(Received);

            }


        }

        public void SerialPort1_DataReceived(object sender, DataReceivedArgs e)
        {
            //robot.receivedText += Encoding.UTF8.GetString(e.Data, 0, e.Data.Length);

            for (int i = 0; i < e.Data.Length; i++)
            {
                robot.byteListReceived.Enqueue(e.Data[i]);


            }

        }


        private void TextBox_TextChanged(object sender, TextChangedEventArgs e)
        {

        }
      /*  private void TextBoxEmission_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {

                //TextBoxréception.Text += ("Reçu : " + textBoxEmission.Text);

                serialPort1.Write(textBoxEmission.Text);

                //receivedText =textBoxEmission.Text ;

                textBoxEmission.Text = "";
            }
        }*/

        private void SendWaypoint(float x, float Y)
        {
          

            byte[] payload = new byte[8];
            byte[] array = BitConverter.GetBytes(x);
            Array.Copy(array, 0, payload, 0, 4);
            array = BitConverter.GetBytes(Y);
            Array.Copy(array, 0, payload, 4, 4);

    
            
            UartEncodeAndSendMessage(0x0081, payload.Length, payload); //1.57   
            XW.Text = "XWaypoint :" + x;
            YW.Text = "YWaypoint :" + Y;
        
        }




        private void Test_Click_Ouest(object sender, RoutedEventArgs e)
        {
            
            SendWaypoint(-1, 0);

            
        }
        
        private void Test_Click_Est(object sender, RoutedEventArgs e)
        {
          
            SendWaypoint(1, 0);
        }
        private void Test_Click_Nord(object sender, RoutedEventArgs e)
        {
        
            SendWaypoint(0, 1);
        }
        private void Test_Click_Sud(object sender, RoutedEventArgs e)
        {
           
            SendWaypoint(0, -1);
        }
        private void Test_Click_NordVpn(object sender, RoutedEventArgs e)
        {
           
            SendWaypoint(1, -1);
        }


       


        private void buttonEnvoyer_Click(object sender, RoutedEventArgs e)
        {

            if (!float.TryParse(Kd.Text, out float valeur))
                return;

          

            if (!float.TryParse(Ki.Text, out float valeur2))
                return;

            if (!float.TryParse(Kp.Text, out float valeur3))

                return;

            if (!float.TryParse(LineaireBox.Text, out float valeur4))

                return;
            if (!float.TryParse(AngulaireBox.Text, out float valeur5))
                return;



            if (!float.TryParse(KpX.Text, out float valeur6))

                return;

            if (!float.TryParse(KiX.Text, out float valeur7))

                return;
            if (!float.TryParse(KdX.Text, out float valeur8))
                return;


         

            List<byte> values = new List<byte>();
            values.AddRange(BitConverter.GetBytes(valeur));
            values.AddRange(BitConverter.GetBytes(valeur2));
            values.AddRange(BitConverter.GetBytes(valeur3));
            values.AddRange(BitConverter.GetBytes(valeur4));
            values.AddRange(BitConverter.GetBytes(valeur5));
            values.AddRange(BitConverter.GetBytes(valeur6));
            values.AddRange(BitConverter.GetBytes(valeur7));
            values.AddRange(BitConverter.GetBytes(valeur8));


            byte[] tableau = values.ToArray();


            asservSpeedDisplay.UpdatePolarSpeedConsigneValues(valeur4, valeur5);
            UartEncodeAndSendMessage(0x67, tableau.Length, tableau);
    






            
            
     
     
            if (toogle == false)
            {
                buttonEnvoyer.Background = Brushes.RoyalBlue;
                toogle = !toogle;
            }
            else
            {
                buttonEnvoyer.Background = Brushes.Beige;
                toogle = !toogle;

            }


           


        }

        /*  private void _globalKeyboardHook_KeyPressed(object? sender, KeyArgs e)
          {
              if (autoControlActivated == false)
              {
                  switch (e.keyCode)
                  {
                      case KeyCode.LEFT:
                          UartEncodeAndSendMessage(0x0051, 1, new byte[] {
                          (byte)StateRobot.STATE_TOURNE_SUR_PLACE_GAUCHE });
                          break;
                      case KeyCode.RIGHT:
                          UartEncodeAndSendMessage(0x0051, 1, new byte[] {
                      (byte)StateRobot.STATE_TOURNE_SUR_PLACE_DROITE });
                          break;
                      case KeyCode.UP:
                          UartEncodeAndSendMessage(0x0051, 1, new byte[]
                          { (byte)StateRobot.STATE_AVANCE });
                          break;
                      case KeyCode.DOWN:
                          UartEncodeAndSendMessage(0x0051, 1, new byte[]
                          { (byte)StateRobot.STATE_ARRET });
                          break;
                      case KeyCode.PAGEDOWN:
                          UartEncodeAndSendMessage(0x0051, 1, new byte[]
                          { (byte)StateRobot.STATE_RECULE });
                          break;




                  }
              }
          }*/
       private void PointWay(double x, double y)
        {

        }
        private void ButtonClear_Click(object sender, RoutedEventArgs e)
        {
            TextBoxréception.Text = "";
        }

    

        private void Test_Click(object sender, RoutedEventArgs e)
        {
            if (!float.TryParse(XInput.Text, out float XInput1))
                return;
            if (!float.TryParse(YInput.Text, out float YInput1))
                return;
            if (!float.TryParse(EcartInput.Text, out float Ecartinput1))
                return;

            if (!float.TryParse(AngKD.Text, out float AngKD1))

                return;

            if (!float.TryParse(AngKP.Text, out float AngKP1))

                return;




            PointWay(XInput1, YInput1);

            List<byte> Input = new List<byte>();
            Input.AddRange(BitConverter.GetBytes(XInput1));
            Input.AddRange(BitConverter.GetBytes(YInput1));
            Input.AddRange(BitConverter.GetBytes(Ecartinput1));
            Input.AddRange(BitConverter.GetBytes(AngKD1));
            Input.AddRange(BitConverter.GetBytes(AngKP1));


            List<byte> Input1 = new List<byte>();
            Input1.AddRange(BitConverter.GetBytes(AngKD1));
            Input1.AddRange(BitConverter.GetBytes(AngKP1));


            byte[] ThetaTab = Input.ToArray();

            byte[] ThetaTab1 = Input1.ToArray();

            UartEncodeAndSendMessage(0x81, ThetaTab.Length, ThetaTab);

            UartEncodeAndSendMessage(0x83, ThetaTab1.Length, ThetaTab1);

        }
        private byte CalculateChecksum(int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {

            byte checksum = 0x00;


            checksum ^= 0xFE;

            checksum ^= 0x00;
            checksum ^= (byte)msgFunction;

            checksum ^= (byte)(msgPayloadLength >> 8);
            checksum ^= (byte)msgPayloadLength;

            for (int i = 0; i < msgPayloadLength; i++)
            {
                checksum ^= msgPayload[i];
                //^= c est un xor la boucle mssgpayload cree un chechsum qui sera comparé a celui calculé précedemement
            }

            return checksum;


        }



        private void TextKp_KeyUp(object sender, KeyEventArgs e)
        {

        }





        void UartEncodeAndSendMessage(int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {
            
          
            
            byte[] trame = new byte[msgPayloadLength + 6];
            int a = 0;
            trame[0] = 0xFE;
            trame[1] = 0x00;
            trame[2] = (byte)(msgFunction);
            trame[3] = (byte)(msgPayloadLength >> 8);
            trame[4] = (byte)(msgPayloadLength);

            for (int i = 0; i < msgPayloadLength; i++)
            {
                trame[5 + i] = (byte)(msgPayload[i]);
                a++;
            }

            trame[5 + a] = CalculateChecksum(msgFunction, msgPayloadLength, msgPayload);
            /* foreach (byte i in trame)
             {

                 TextBoxréception.Text += i;
             }
             */
            serialPort1.Write(trame, 0, trame.Length);

        }
      

        public enum StateRobot
        {
            STATE_ATTENTE = 0,
            STATE_ATTENTE_EN_COURS = 1,
            STATE_AVANCE = 2,
            STATE_AVANCE_EN_COURS = 3,
            STATE_TOURNE_GAUCHE = 4,
            STATE_TOURNE_GAUCHE_EN_COURS = 5,
            STATE_TOURNE_DROITE = 6,
            STATE_TOURNE_DROITE_EN_COURS = 7,
            STATE_TOURNE_SUR_PLACE_GAUCHE = 8,
            STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS = 9,
            STATE_TOURNE_SUR_PLACE_DROITE = 10,
            STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS = 11,
            STATE_ARRET = 12,
            STATE_ARRET_EN_COURS = 13,
            STATE_RECULE = 14,
            STATE_RECULE_EN_COURS = 15
        }

   

       




        void ProcessDecodedMessage(int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {
            switch ((StateMessage)msgFunction)
            {
                case StateMessage.Text:
                    TextBoxréception.Text = "Text recu : " + Encoding.UTF8.GetString(msgPayload, 0, msgPayload.Length);


                    break;
                case StateMessage.Led:

                    if (msgPayload[0] == 1)
                        if (msgPayload[1] == 1)
                            Led1.IsChecked = true;
                        else
                            Led1.IsChecked = false;
                    if (msgPayload[0] == 2)
                        if (msgPayload[1] == 1)
                            Led2.IsChecked = true;
                        else
                            Led2.IsChecked = false;
                    if (msgPayload[0] == 3)
                        if (msgPayload[1] == 1)
                            Led3.IsChecked = true;
                        else
                            Led3.IsChecked = false;
                    break;

                case StateMessage.IRDistance:
                    IRG.Text = "IR Gauche : " + BitConverter.ToSingle(msgPayload, 0).ToString("N1") + " cm";
                    IRC.Text = "IR Centre : " + BitConverter.ToSingle(msgPayload, 4).ToString("N1") + " cm";
                    IRD.Text = "IR Droite : " + BitConverter.ToSingle(msgPayload, 8).ToString("N1") + " cm";
                    break;
                case StateMessage.Moteur:
                    MG.Text = "Vitesse Gauche : " + msgPayload[0] + "%";
                    MD.Text = "Vitesse Droite : " + msgPayload[1] + "%";

                    break;
                case StateMessage.Step:
                    int instant = (((int)msgPayload[1]) << 24) + (((int)msgPayload[2]) << 16)
                    + (((int)msgPayload[3]) << 8) + ((int)msgPayload[4]);
                    TextBoxréception.Text += "\nRobot␣State␣:␣" +
                    ((StateRobot)(msgPayload[0])).ToString() +
                    "␣-␣" + instant.ToString() + "␣ms";
                    break;
                case StateMessage.Encodeur:
                    robot.positionXOdo = BitConverter.ToSingle(msgPayload, 4);
                    robot.positionYOdo = BitConverter.ToSingle(msgPayload, 8);
                    robot.angleRadianFromOdometry = BitConverter.ToSingle(msgPayload, 12);
                    robot.vitesseLineaireFromOdometry = BitConverter.ToSingle(msgPayload, 16);
                    robot.vitesseAngulaireFromOdometry = BitConverter.ToSingle(msgPayload, 20);
                    X.Text = "X : " + robot.positionXOdo.ToString("N3") + " m";
                    Y.Text = "Y : " + robot.positionYOdo.ToString("N3") + " m";
                    angle.Text = "θ : " + robot.angleRadianFromOdometry.ToString("N3") + " rad";
                    asservSpeedDisplay.UpdatePolarOdometrySpeed(robot.vitesseLineaireFromOdometry, robot.vitesseAngulaireFromOdometry);        
                    
                    break;
                case StateMessage.PID_Verifiy:
                    float Kd, Ki, Kp;
                    Kp = BitConverter.ToSingle(msgPayload, 0);
                    Ki = BitConverter.ToSingle(msgPayload, 4);
                    Kd = BitConverter.ToSingle(msgPayload, 8);
                    asservSpeedDisplay.UpdatePolarSpeedCorrectionGains(Kp, 0, Ki,0, Kd, 0);
                    break;
                case StateMessage.Corr_Pid_Variables:
                
                    float erreurX = BitConverter.ToSingle(msgPayload, 0);
                    float CommandX = BitConverter.ToSingle(msgPayload, 4);
                    float KpX = BitConverter.ToSingle(msgPayload, 8);
                    float CorrPX = BitConverter.ToSingle(msgPayload, 12);
                    float erreurPMaxX = BitConverter.ToSingle(msgPayload,16);
                    float KiX = BitConverter.ToSingle(msgPayload, 20);
                    float CorrIX = BitConverter.ToSingle(msgPayload, 24);
                    float erreurIMaxX = BitConverter.ToSingle(msgPayload, 28);
                    float KdX = BitConverter.ToSingle(msgPayload, 32);
                    float CorrDX = BitConverter.ToSingle(msgPayload, 36);
                    float erreurDMaxX = BitConverter.ToSingle(msgPayload, 40);

               

                    asservSpeedDisplay.UpdatePolarSpeedErrorValues(erreurX, 0);
                    asservSpeedDisplay.UpdatePolarSpeedCommandValues(CommandX, 0);
                    asservSpeedDisplay.UpdatePolarSpeedCorrectionGains(KpX, 0, KiX, 0, KdX, 0);
                    asservSpeedDisplay.UpdatePolarSpeedCorrectionValues(CorrPX, 0, CorrIX, 0, CorrDX, 0);
                    asservSpeedDisplay.UpdatePolarSpeedCorrectionLimits(erreurPMaxX, 0, erreurIMaxX, 0, erreurDMaxX, 0);


                    break;

                case StateMessage.Corr_Pid_VariablesTheta:
                         float erreurT = BitConverter.ToSingle(msgPayload, 44);
                    float CommandT = BitConverter.ToSingle(msgPayload, 48);
                    float KpT = BitConverter.ToSingle(msgPayload, 52);
                    float CorrPT = BitConverter.ToSingle(msgPayload, 56);
                    float erreurPMaxT = BitConverter.ToSingle(msgPayload, 60);
                    float KiT = BitConverter.ToSingle(msgPayload, 64);
                    float CorrIT = BitConverter.ToSingle(msgPayload, 68);
                    float erreurIMaxT = BitConverter.ToSingle(msgPayload, 72);
                    float KdT = BitConverter.ToSingle(msgPayload, 76);
                    float CorrDT = BitConverter.ToSingle(msgPayload, 80);
                    float erreurDMaxT = BitConverter.ToSingle(msgPayload, 84);
                    break;

                case StateMessage.Ghost:


                    float ghost = BitConverter.ToSingle(msgPayload, 0);
                    XG.Text = "XGhost : " + BitConverter.ToSingle(msgPayload, 0);
                    YG.Text = "YGhost : " + BitConverter.ToSingle(msgPayload, 4);
          
                    ThetaG.Text= "ThetaGhost :"+ BitConverter.ToSingle(msgPayload, 20).ToString("N3");
                    RotateRobot(BitConverter.ToSingle(msgPayload, 8));
                    //TextBoxréception.Text = BitConverter.ToSingle(msgPayload, 12).ToString("N3"); 
                    //TextBoxréception.Text = BitConverter.ToSingle(msgPayload, 16).ToString("N3");




                    break;

                case StateMessage.GhostLong:
                    Avance(BitConverter.ToSingle(msgPayload, 0), BitConverter.ToSingle(msgPayload, 4));
                    TextBoxréception.Text = BitConverter.ToSingle(msgPayload, 8).ToString("N3");


                    break;
                case StateMessage.ecart:
                    Ellipse(BitConverter.ToSingle(msgPayload, 0), BitConverter.ToSingle(msgPayload, 4));

                    break;

            }



        }

        private void RotateRobot(double theta)
        {
            double nouvelleAngle = -theta;

            DoubleAnimation animation = new DoubleAnimation
            {
                To = nouvelleAngle,
                Duration = TimeSpan.FromMilliseconds(100),
              
            };

            RotationGhost.BeginAnimation(
                RotateTransform.AngleProperty,
                animation);

            lastAngle = nouvelleAngle;
        }

        private void Avance(double x, double y)
        {
            const double pixelsParMetre = 50;

            double nouvelleX = x * pixelsParMetre;
            double nouvelleY = -y * pixelsParMetre;

            DoubleAnimation animationX = new DoubleAnimation
            {
                To = nouvelleX,
                Duration = TimeSpan.FromMilliseconds(25)
            };

            DoubleAnimation animationY = new DoubleAnimation
            {
                To = nouvelleY,
                Duration = TimeSpan.FromMilliseconds(25)
            };

            TranslationGhost.BeginAnimation(
                TranslateTransform.XProperty,
                animationX);

            TranslationGhost.BeginAnimation(
                TranslateTransform.YProperty,
                animationY);
        }

        private void Ellipse(double x, double y)
        {
            double pixelsParMetre = 50;

            // Si un point existe déjà, on le supprime
            if (waypointPoint != null)
            {
                myGrid.Children.Remove(waypointPoint);
            }

            // Création du nouveau point
            waypointPoint = new Ellipse
            {
                Width = 10,
                Height = 10,
                Fill = Brushes.Red
            };

            double pixelX = myGrid.ActualWidth / 2 + x * pixelsParMetre;
            double pixelY = myGrid.ActualHeight / 2 - y * pixelsParMetre;

            Canvas.SetLeft(waypointPoint, pixelX - waypointPoint.Width / 2);
            Canvas.SetTop(waypointPoint, pixelY - waypointPoint.Height / 2);

            myGrid.Children.Add(waypointPoint);
        }









        public enum StateMessage : int
        {
            Text = 0x0080,
            Led = 0x0020,
            IRDistance = 0x0030,
            Step = 0x0050,
            PID,
            PI,
            P,
            Moteur = 0x0040,
            Encodeur = 0x0061,
            PID_Verifiy= 0x0068,
            Corr_Pid_Variables = 0x0069,
            Corr_Pid_VariablesTheta = 0x0070,
            Ghost = 0x0081,
            GhostLong = 0x0082,
            ecart = 0x0083,





        }


        public enum StateReception
        {
            Waiting,
            FunctionMSB,
            FunctionLSB,
            PayloadLengthMSB,
            PayloadLengthLSB,
            Payload,
            CheckSum
        }
        StateReception rcvState = StateReception.Waiting;
        int msgDecodedFunction = 0;

        int msgDecodedPayloadLength = 0;
        byte[] msgDecodedPayload;
        int msgDecodedPayloadIndex = 0;

     

        private void DecodeMessage(byte c)
        {

            switch (rcvState)
            {
                case StateReception.Waiting:

                    if (c == 0xFE)
                        rcvState = StateReception.FunctionMSB;


                    break;
                case StateReception.FunctionMSB:
                    msgDecodedFunction = c;
                    rcvState = StateReception.FunctionLSB;

                    break;
                case StateReception.FunctionLSB:
                    msgDecodedFunction += c;

                    rcvState = StateReception.PayloadLengthMSB;
                    break;
                case StateReception.PayloadLengthMSB:
                    msgDecodedPayloadLength = (c << 8);
                    rcvState = StateReception.PayloadLengthLSB;



                    break;
                case StateReception.PayloadLengthLSB:
                    msgDecodedPayloadLength += c;
                    if (msgDecodedPayloadLength == 0)
                        rcvState = StateReception.CheckSum;
                    else if (msgDecodedPayloadLength < 1024)
                    {
                        msgDecodedPayload = new byte[msgDecodedPayloadLength];
                        rcvState = StateReception.Payload;
                    }
                    else
                    {
                        rcvState = StateReception.Waiting;
                    }
                    break;
                case StateReception.Payload:

                    msgDecodedPayload[msgDecodedPayloadIndex] = c;
                    msgDecodedPayloadIndex++;
                    if (msgDecodedPayloadIndex >= msgDecodedPayloadLength)
                    {
                        rcvState = StateReception.CheckSum;
                        msgDecodedPayloadIndex = 0;

                    }


                    break;
                case StateReception.CheckSum:
                    byte calculatedChecksum = CalculateChecksum(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);

                    if (calculatedChecksum == c)
                    {
                        ProcessDecodedMessage(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);

                    }
                    rcvState = StateReception.Waiting;

                    break;
                default:
                    rcvState = StateReception.Waiting;
                    break;
            }
        }



    }

}
