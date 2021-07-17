package com.example.androidex;

import java.util.concurrent.TimeUnit;

import com.example.androidex.R;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Color;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;



public class MultiPlayer extends Activity{

	public native int readjni();	// Read input switch
	public native void fndjni(int n);	// Output Current Players total Score
	public native void dotjni(int n);	// Most numbers currently out
	public native long gettime();	// get current time by system call

	Handler mHandler=new Handler(){
		public void handleMessage(Message msg){
			if(msg.what==0){
				playtime=gettime();
				target = (TextView)findViewById(R.id.playtime);
				long temp = (starttime - playtime);
				if(temp<=0){
					temp=0;
					if(dice[0] != -1){
						Save();
						Change_User();
						if(round==13)
							end_game();
					}
				}
				if(temp ==0)
					target.setText("");
				else
					target.setText("Left Time: "+ Long.toString(temp) + "Seconds");
				if(msg.arg1==1){
					UP();
				}
				else if(msg.arg1==2){
					DOWN();
				}
				else if(msg.arg1==3){
					if(dice[0]!=-1){
						Save();
						Change_User();
						if(round==13){
							end_game();
						}
					}
				}
				else if(msg.arg1==4){
					if(left_roll!=0)
						Throw();
				}
			}
		}
	};

	class BackThread extends Thread{
		Handler sHandler;

		BackThread(Handler handler){
			sHandler=handler;
		}
		public void run(){
			int n=0;
			while(!isInterrupted()){
				n=0;
				n = readjni();
				Message msg=Message.obtain();
				msg.what=0;
				msg.arg1=n;
				sHandler.sendMessage(msg);
				try{Thread.sleep(300);}
				catch(InterruptedException e)
				{Thread.currentThread( ).interrupt( );}
			}
		}
	}



	static class Player{
		private int saved[];
		private int Aces;
		private int Duces;
		private int Threes;
		private int Fours;
		private int Fives;
		private int Sixes;

		private int subtotal;
		private int Choice;
		private int fourofakind;
		private int fullhouse;
		private int smallstraight;
		private int largestraight;
		private int yacht;
		private int total;

		Player(){
			this.saved = new int[12];
			for(int i=0; i<12;i++)
				this.saved[i]=0;
			this.Aces=0;	// 0
			this.Duces=0;	// 1
			this.Threes=0;	// 2
			this.Fours=0;	// 3
			this.Fives=0;	// 4
			this.Sixes=0;	// 5
			this.subtotal=0;
			this.Choice=0;	// 6
			this.fourofakind=0; // 7
			this.fullhouse=0;	// 8
			this.smallstraight=0;	// 9
			this.largestraight=0;	// 10
			this.yacht=0;	// 11
			this.total=0;	
		}

	}
	private int curr_player;
	private int round;
	private int left_roll;
	private int dice[];
	private int dice_status[];
	private int curr_location;
	private int winner;
	private long starttime;
	private long playtime;

	BackThread mThread;
	Player p[];
	ImageView temp;
	RelativeLayout BG;
	TextView target;

	@Override
	protected void onCreate(Bundle savedInstanceState) {		
		Button btn1,btn2;
		ImageButton imagebtn1,imagebtn2,imagebtn3,imagebtn4,imagebtn5, imagebtn6,imagebtn7,imagebtn8,imagebtn9,imagebtn10;
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main2);

		System.loadLibrary("readjni");

		p = new Player[2];
		p[0] = new Player();
		p[1] = new Player();

		round = 1;
		curr_player = 0;
		dice = new int[5];
		dice_status = new int [5];
		curr_location=0;
		left_roll=3;

		initial_dice();

		// Buttons // 

		btn1=(Button)findViewById(R.id.Throw);
		OnClickListener listener=new OnClickListener(){
			public void onClick(View v){
				if(left_roll!=0)
					Throw();
			}
		};
		btn1.setOnClickListener(listener);

		btn2=(Button)findViewById(R.id.endgame);
		OnClickListener listener2=new OnClickListener(){
			public void onClick(View v){
				fndjni(0);
				dotjni(0);
				finish();
				mThread.interrupt();
			}
		};
		btn2.setOnClickListener(listener2);


		imagebtn1=(ImageButton)findViewById(R.id.RolledDice1);
		OnClickListener listener3=new OnClickListener(){
			public void onClick(View v){
				if(dice_status[0] == 0){
					temp = (ImageView)findViewById(R.id.RolledDice1);
					temp.setImageResource(R.drawable.saved); 

					temp = (ImageView)findViewById(R.id.SavedDice1);
					temp.setImageResource(R.drawable.dice_saved_1+dice[0]-1); 

					dice_status[0]=1;
				}
			}
		};
		imagebtn1.setOnClickListener(listener3);

		imagebtn2=(ImageButton)findViewById(R.id.RolledDice2);
		OnClickListener listener4=new OnClickListener(){
			public void onClick(View v){
				if(dice_status[1] == 0){
					temp = (ImageView)findViewById(R.id.RolledDice2);
					temp.setImageResource(R.drawable.saved); 

					temp = (ImageView)findViewById(R.id.SavedDice2);
					temp.setImageResource(R.drawable.dice_saved_1+dice[1]-1); 

					dice_status[1]=1;
				}
			}
		};
		imagebtn2.setOnClickListener(listener4);

		imagebtn3=(ImageButton)findViewById(R.id.RolledDice3);
		OnClickListener listener5=new OnClickListener(){
			public void onClick(View v){
				if(dice_status[2] == 0){
					temp = (ImageView)findViewById(R.id.RolledDice3);
					temp.setImageResource(R.drawable.saved); 

					temp = (ImageView)findViewById(R.id.SavedDice3);
					temp.setImageResource(R.drawable.dice_saved_1+dice[2]-1); 

					dice_status[2]=1;
				}
			}
		};
		imagebtn3.setOnClickListener(listener5);

		imagebtn4=(ImageButton)findViewById(R.id.RolledDice4);
		OnClickListener listener6=new OnClickListener(){
			public void onClick(View v){
				if(dice_status[3] == 0){
					temp = (ImageView)findViewById(R.id.RolledDice4);
					temp.setImageResource(R.drawable.saved); 

					temp = (ImageView)findViewById(R.id.SavedDice4);
					temp.setImageResource(R.drawable.dice_saved_1+dice[3]-1); 

					dice_status[3]=1;
				}
			}
		};
		imagebtn4.setOnClickListener(listener6);

		imagebtn5=(ImageButton)findViewById(R.id.RolledDice5);
		OnClickListener listener7=new OnClickListener(){
			public void onClick(View v){
				if(dice_status[4] == 0){
					temp = (ImageView)findViewById(R.id.RolledDice5);
					temp.setImageResource(R.drawable.saved); 

					temp = (ImageView)findViewById(R.id.SavedDice5);
					temp.setImageResource(R.drawable.dice_saved_1+dice[4]-1); 

					dice_status[4]=1;
				}
			}
		};
		imagebtn5.setOnClickListener(listener7);

		imagebtn6=(ImageButton)findViewById(R.id.SavedDice1);
		OnClickListener listener8=new OnClickListener(){
			public void onClick(View v){
				if(dice_status[0] == 1){
					temp = (ImageView)findViewById(R.id.SavedDice1);
					temp.setImageResource(R.drawable.empty); 

					temp = (ImageView)findViewById(R.id.RolledDice1);
					temp.setImageResource(R.drawable.dice_1+dice[0]-1); 

					dice_status[0]=0;
				}
			}
		};
		imagebtn6.setOnClickListener(listener8);

		imagebtn7=(ImageButton)findViewById(R.id.SavedDice2);
		OnClickListener listener9=new OnClickListener(){
			public void onClick(View v){
				if(dice_status[1] == 1){
					temp = (ImageView)findViewById(R.id.SavedDice2);
					temp.setImageResource(R.drawable.empty); 

					temp = (ImageView)findViewById(R.id.RolledDice2);
					temp.setImageResource(R.drawable.dice_1+dice[1]-1); 

					dice_status[1]=0;
				}
			}
		};
		imagebtn7.setOnClickListener(listener9);

		imagebtn8=(ImageButton)findViewById(R.id.SavedDice3);
		OnClickListener listener10=new OnClickListener(){
			public void onClick(View v){
				if(dice_status[2] == 1){
					temp = (ImageView)findViewById(R.id.SavedDice3);
					temp.setImageResource(R.drawable.empty); 

					temp = (ImageView)findViewById(R.id.RolledDice3);
					temp.setImageResource(R.drawable.dice_1+dice[2]-1); 

					dice_status[2]=0;
				}
			}
		};
		imagebtn8.setOnClickListener(listener10);

		imagebtn9=(ImageButton)findViewById(R.id.SavedDice4);
		OnClickListener listener11=new OnClickListener(){
			public void onClick(View v){
				if(dice_status[3] == 1){
					temp = (ImageView)findViewById(R.id.SavedDice4);
					temp.setImageResource(R.drawable.empty); 

					temp = (ImageView)findViewById(R.id.RolledDice4);
					temp.setImageResource(R.drawable.dice_1+dice[3]-1); 

					dice_status[3]=0;
				}
			}
		};
		imagebtn9.setOnClickListener(listener11);

		imagebtn10=(ImageButton)findViewById(R.id.SavedDice5);
		OnClickListener listener12=new OnClickListener(){
			public void onClick(View v){
				if(dice_status[4] == 1){
					temp = (ImageView)findViewById(R.id.SavedDice5);
					temp.setImageResource(R.drawable.empty); 

					temp = (ImageView)findViewById(R.id.RolledDice5);
					temp.setImageResource(R.drawable.dice_1+dice[4]-1); 

					dice_status[4]=0;
				}
			}
		};
		imagebtn10.setOnClickListener(listener12);
		mThread=new BackThread(mHandler);
		mThread.setDaemon(true);
		mThread.start();
	}

	protected void UP(){
		if(dice[0]!=-1){
			int prev_location = curr_location;
			for(int i=1;i<12;i++){
				if(p[curr_player].saved[(12 + curr_location-i)%12]==0){
					curr_location = (12 + curr_location-i)%12;
					break;
				}
			}
			turn_off(prev_location);
			turn_on(curr_location);
			Print_Estimation(curr_player);
		}
	}
	protected void DOWN(){
		if(dice[0] != -1){
			int prev_location = curr_location;
			for(int i=1;i<12;i++){
				if(p[curr_player].saved[(curr_location+i)%12]==0){
					curr_location = (curr_location+i)%12;
					break;
				}
			}
			turn_off(prev_location);
			turn_on(curr_location);
			Print_Estimation(curr_player);
		}
	}

	protected void end_game(){
		if(p[0].subtotal >= 63)
			p[0].total+=35;
		if(p[1].subtotal >=63)
			p[1].total+=35;

		if(p[0].total < p[1].total)
			winner=1;
		else if (p[0].total==p[1].total)
			winner=2;

		int player[][] = new int [2][15];
		for(int i=0;i<2;i++){
			player[i][0] = p[i].Aces;
			player[i][1] = p[i].Duces;
			player[i][2] = p[i].Threes;
			player[i][3] = p[i].Fours;
			player[i][4] = p[i].Fives;
			player[i][5] = p[i].Sixes;
			player[i][6] = p[i].Choice;
			player[i][7] = p[i].fourofakind;
			player[i][8] = p[i].fullhouse;
			player[i][9] = p[i].smallstraight;
			player[i][10] = p[i].largestraight;
			player[i][11] = p[i].yacht;
			player[i][12] = p[i].total;
			player[i][13] = p[i].subtotal;
			if(p[i].subtotal >= 63)
				player[i][14] = 35;
			else
				player[i][14] = 0;
		}
		Intent intent=new Intent(MultiPlayer.this, Result_View.class);
		intent.putExtra("winner", Integer.toString(winner));
		intent.putExtra("player1", player[0]);
		intent.putExtra("player2", player[1]);

		startActivity(intent);

		fndjni(0);
		dotjni(0);
		mThread.interrupt();
		finish();
	}
	protected void turn_off(int n){
		if(curr_player==0){
			if(n==0){
				target = (TextView)findViewById(R.id.Aces1);
				target.setBackgroundColor(0);	
			}
			else if(n==1){
				target = (TextView)findViewById(R.id.Duces1);
				target.setBackgroundColor(0);	
			}
			else if(n==2){
				target = (TextView)findViewById(R.id.Threes1);
				target.setBackgroundColor(0);	
			}
			else if(n==3){
				target = (TextView)findViewById(R.id.Fours1);
				target.setBackgroundColor(0);	
			}
			else if(n==4){
				target = (TextView)findViewById(R.id.Fives1);
				target.setBackgroundColor(0);	
			}
			else if(n==5){
				target = (TextView)findViewById(R.id.Sixes1);
				target.setBackgroundColor(0);	
			}
			else if(n==6){
				target = (TextView)findViewById(R.id.Choice1);
				target.setBackgroundColor(0);	
			}
			else if(n==7){
				target = (TextView)findViewById(R.id.fourofakind1);
				target.setBackgroundColor(0);	
			}
			else if(n==8){
				target = (TextView)findViewById(R.id.fullhouse1);
				target.setBackgroundColor(0);	
			}
			else if(n==9){
				target = (TextView)findViewById(R.id.smallstraight1);
				target.setBackgroundColor(0);	
			}
			else if(n==10){
				target = (TextView)findViewById(R.id.largestraight1);
				target.setBackgroundColor(0);	
			}
			else if(n==11){
				target = (TextView)findViewById(R.id.yacht1);
				target.setBackgroundColor(0);	
			}
		}
		else{
			if(n==0){
				target = (TextView)findViewById(R.id.Aces2);
				target.setBackgroundColor(0);	
			}
			else if(n==1){
				target = (TextView)findViewById(R.id.Duces2);
				target.setBackgroundColor(0);	
			}
			else if(n==2){
				target = (TextView)findViewById(R.id.Threes2);
				target.setBackgroundColor(0);	
			}
			else if(n==3){
				target = (TextView)findViewById(R.id.Fours2);
				target.setBackgroundColor(0);	
			}
			else if(n==4){
				target = (TextView)findViewById(R.id.Fives2);
				target.setBackgroundColor(0);	
			}
			else if(n==5){
				target = (TextView)findViewById(R.id.Sixes2);
				target.setBackgroundColor(0);	
			}
			else if(n==6){
				target = (TextView)findViewById(R.id.Choice2);
				target.setBackgroundColor(0);	
			}
			else if(n==7){
				target = (TextView)findViewById(R.id.fourofakind2);
				target.setBackgroundColor(0);	
			}
			else if(n==8){
				target = (TextView)findViewById(R.id.fullhouse2);
				target.setBackgroundColor(0);	
			}
			else if(n==9){
				target = (TextView)findViewById(R.id.smallstraight2);
				target.setBackgroundColor(0);	
			}
			else if(n==10){
				target = (TextView)findViewById(R.id.largestraight2);
				target.setBackgroundColor(0);	
			}
			else if(n==11){
				target = (TextView)findViewById(R.id.yacht2);
				target.setBackgroundColor(0);	
			}
		}
	}
	protected void turn_on(int n){
		if(curr_player==0){
			if(n==0){
				target = (TextView)findViewById(R.id.Aces1);
				target.setBackgroundColor(Color.parseColor("#1ae89c"));	
			}
			else if(n==1){
				target = (TextView)findViewById(R.id.Duces1);
				target.setBackgroundColor(Color.parseColor("#1ae89c"));	
			}
			else if(n==2){
				target = (TextView)findViewById(R.id.Threes1);
				target.setBackgroundColor(Color.parseColor("#1ae89c"));	
			}
			else if(n==3){
				target = (TextView)findViewById(R.id.Fours1);
				target.setBackgroundColor(Color.parseColor("#1ae89c"));	
			}
			else if(n==4){
				target = (TextView)findViewById(R.id.Fives1);
				target.setBackgroundColor(Color.parseColor("#1ae89c"));	
			}
			else if(n==5){
				target = (TextView)findViewById(R.id.Sixes1);
				target.setBackgroundColor(Color.parseColor("#1ae89c"));	
			}
			else if(n==6){
				target = (TextView)findViewById(R.id.Choice1);
				target.setBackgroundColor(Color.parseColor("#1ae89c"));	
			}
			else if(n==7){
				target = (TextView)findViewById(R.id.fourofakind1);
				target.setBackgroundColor(Color.parseColor("#1ae89c"));	
			}
			else if(n==8){
				target = (TextView)findViewById(R.id.fullhouse1);
				target.setBackgroundColor(Color.parseColor("#1ae89c"));	
			}
			else if(n==9){
				target = (TextView)findViewById(R.id.smallstraight1);
				target.setBackgroundColor(Color.parseColor("#1ae89c"));	
			}
			else if(n==10){
				target = (TextView)findViewById(R.id.largestraight1);
				target.setBackgroundColor(Color.parseColor("#1ae89c"));	
			}
			else if(n==11){
				target = (TextView)findViewById(R.id.yacht1);
				target.setBackgroundColor(Color.parseColor("#1ae89c"));	
			}
		}
		else{
			if(n==0){
				target = (TextView)findViewById(R.id.Aces2);
				target.setBackgroundColor(Color.parseColor("#1ae89c"));	
			}
			else if(n==1){
				target = (TextView)findViewById(R.id.Duces2);
				target.setBackgroundColor(Color.parseColor("#1ae89c"));	
			}
			else if(n==2){
				target = (TextView)findViewById(R.id.Threes2);
				target.setBackgroundColor(Color.parseColor("#1ae89c"));	
			}
			else if(n==3){
				target = (TextView)findViewById(R.id.Fours2);
				target.setBackgroundColor(Color.parseColor("#1ae89c"));	
			}
			else if(n==4){
				target = (TextView)findViewById(R.id.Fives2);
				target.setBackgroundColor(Color.parseColor("#1ae89c"));	
			}
			else if(n==5){
				target = (TextView)findViewById(R.id.Sixes2);
				target.setBackgroundColor(Color.parseColor("#1ae89c"));	
			}
			else if(n==6){
				target = (TextView)findViewById(R.id.Choice2);
				target.setBackgroundColor(Color.parseColor("#1ae89c"));	
			}
			else if(n==7){
				target = (TextView)findViewById(R.id.fourofakind2);
				target.setBackgroundColor(Color.parseColor("#1ae89c"));	
			}
			else if(n==8){
				target = (TextView)findViewById(R.id.fullhouse2);
				target.setBackgroundColor(Color.parseColor("#1ae89c"));	
			}
			else if(n==9){
				target = (TextView)findViewById(R.id.smallstraight2);
				target.setBackgroundColor(Color.parseColor("#1ae89c"));	
			}
			else if(n==10){
				target = (TextView)findViewById(R.id.largestraight2);
				target.setBackgroundColor(Color.parseColor("#1ae89c"));	
			}
			else if(n==11){
				target = (TextView)findViewById(R.id.yacht2);
				target.setBackgroundColor(Color.parseColor("#1ae89c"));	
			}
		}
	}
	protected void initial_dice(){
		for(int i=0;i<5;i++){
			dice_status[i]=-1;
			dice[i]=-1;
			temp = (ImageView)findViewById(R.id.SavedDice1+i);
			temp.setImageResource(R.drawable.empty); 
			temp = (ImageView)findViewById(R.id.RolledDice1+i);
			temp.setImageResource(R.drawable.initial); 
		}
		dotjni(0);
		fndjni(0);
	}

	protected void Throw(){
		for(int i=0;i<5;i++){
			if(dice_status[i] == 1)
				continue;
			if(dice_status[i]==-1)
				dice_status[i]=0;
			int random_n = (int) (Math.random()*1000)%6;
			temp = (ImageView)findViewById(R.id.RolledDice1 + i);
			temp.setImageResource(R.drawable.dice_1 + random_n);
			dice[i] = random_n+1;
		}
		int temp_count[] = new int[6];
		for(int i=0;i<6;i++)
			temp_count[i]=0;
		for(int i=0;i<5;i++){
			temp_count[dice[i]-1]++;
		}
		int temp_max =-1;
		int temp_max_index=0;
		for(int i=0;i<6;i++){
			if(temp_max < temp_count[i]){
				temp_max=temp_count[i];
				temp_max_index=i;
			}
		}
		for(int i=0;i<6;i++){
			if(temp_count[i] == temp_max && temp_max_index != i){
				temp_max_index=10;
				break;
			}
		}
		dotjni(temp_max_index+1);



		if(get_yacht()==50){
			MediaPlayer mp2;
			mp2=MediaPlayer.create(this,R.raw.yacht);
			mp2.start();
		}
		Print_Estimation(curr_player);
		turn_off(curr_location);
		for(int i=0;i<12;i++)
			if(p[curr_player].saved[i]==0){
				curr_location = i;
				turn_on(i);
				break;
			}
		
		starttime=gettime()+30;
		left_roll--;
		target = (TextView)findViewById(R.id.Turns);
		target.setText(Integer.toString(left_roll)+" left");
	}

	protected void Save(){
		dotjni(0);
		starttime=gettime();
		p[curr_player].saved[curr_location]=1;
		turn_off(curr_location);
		if (curr_location==0){
			p[curr_player].Aces = get_plane(1);
			p[curr_player].total += get_plane(1);
			p[curr_player].subtotal += get_plane(1);
		}
		else if(curr_location==1){
			p[curr_player].Duces = get_plane(2);
			p[curr_player].total += get_plane(2);
			p[curr_player].subtotal += get_plane(2);
		}
		else if(curr_location==2){
			p[curr_player].Threes = get_plane(3);
			p[curr_player].total += get_plane(3);
			p[curr_player].subtotal += get_plane(3);
		}
		else if(curr_location==3){
			p[curr_player].Fours = get_plane(4);
			p[curr_player].total += get_plane(4);
			p[curr_player].subtotal += get_plane(4);
		}
		else if(curr_location==4){
			p[curr_player].Fives = get_plane(5);
			p[curr_player].total += get_plane(5);
			p[curr_player].subtotal += get_plane(5);
		}
		else if(curr_location==5){
			p[curr_player].Sixes = get_plane(6);
			p[curr_player].total += get_plane(6);
			p[curr_player].subtotal += get_plane(6);
		}
		else if(curr_location==6){
			p[curr_player].Choice = get_choice();
			p[curr_player].total += get_choice();
		}
		else if(curr_location==7){
			p[curr_player].fourofakind = get_fourofakind();
			p[curr_player].total += get_fourofakind();
		}
		else if(curr_location==8){
			p[curr_player].fullhouse = get_fullhouse();
			p[curr_player].total += get_fullhouse();
		}
		else if(curr_location==9){
			p[curr_player].smallstraight = get_smallstraight();
			p[curr_player].total += get_smallstraight();
		}
		else if(curr_location==10){
			p[curr_player].largestraight = get_largestraight();
			p[curr_player].total += get_largestraight();
		}
		else if(curr_location==11){
			p[curr_player].yacht = get_yacht();
			p[curr_player].total += get_yacht();
		}
	}

	protected void Change_User(){
		left_roll=3;
		if(curr_player==0){
			curr_player=1;
			BG = (RelativeLayout)findViewById(R.id.Background);
			BG.setBackgroundResource(R.drawable.board2);
		}
		else{
			curr_player=0;
			BG = (RelativeLayout)findViewById(R.id.Background);
			BG.setBackgroundResource(R.drawable.board1);
			round++;
		}
		target = (TextView)findViewById(R.id.Rounds);
		target.setText(Integer.toString(round)+"/12");

		target = (TextView)findViewById(R.id.Turns);
		target.setText(Integer.toString(left_roll)+" left");
		Print_Raw();
		initial_dice();
		fndjni(p[curr_player].total);


	}


	protected void Print_Estimation(int curr){
		Print_Raw();

		if(p[curr].saved[0]==0){
			if(curr==0)
				target = (TextView)findViewById(R.id.Aces1);
			else
				target = (TextView)findViewById(R.id.Aces2);
			target.setText(Integer.toString(get_plane(1)));
			target.setTextColor(Color.parseColor("#939995"));
		}

		if(p[curr].saved[1]==0){
			if(curr==0)
				target = (TextView)findViewById(R.id.Duces1);
			else
				target = (TextView)findViewById(R.id.Duces2);
			target.setText(Integer.toString(get_plane(2)));
			target.setTextColor(Color.parseColor("#939995"));
		}

		if(p[curr].saved[2]==0){
			if(curr==0)
				target = (TextView)findViewById(R.id.Threes1);
			else
				target = (TextView)findViewById(R.id.Threes2);
			target.setText(Integer.toString(get_plane(3)));
			target.setTextColor(Color.parseColor("#939995"));
		}

		if(p[curr].saved[3]==0){
			if(curr==0)
				target = (TextView)findViewById(R.id.Fours1);
			else
				target = (TextView)findViewById(R.id.Fours2);
			target.setText(Integer.toString(get_plane(4)));
			target.setTextColor(Color.parseColor("#939995"));
		}

		if(p[curr].saved[4]==0){
			if(curr==0)
				target = (TextView)findViewById(R.id.Fives1);
			else
				target = (TextView)findViewById(R.id.Fives2);
			target.setText(Integer.toString(get_plane(5)));
			target.setTextColor(Color.parseColor("#939995"));
		}

		if(p[curr].saved[5]==0){
			if(curr==0)
				target = (TextView)findViewById(R.id.Sixes1);
			else
				target = (TextView)findViewById(R.id.Sixes2);
			target.setText(Integer.toString(get_plane(6)));
			target.setTextColor(Color.parseColor("#939995"));
		}

		if(p[curr].saved[6]==0){
			if(curr==0)
				target = (TextView)findViewById(R.id.Choice1);
			else
				target = (TextView)findViewById(R.id.Choice2);
			target.setText(Integer.toString(get_choice()));
			target.setTextColor(Color.parseColor("#939995"));
		}

		if(p[curr].saved[7]==0){
			if(curr==0)
				target = (TextView)findViewById(R.id.fourofakind1);
			else
				target = (TextView)findViewById(R.id.fourofakind2);
			target.setText(Integer.toString(get_fourofakind()));
			target.setTextColor(Color.parseColor("#939995"));
		}

		if(p[curr].saved[8]==0){
			if(curr==0)
				target = (TextView)findViewById(R.id.fullhouse1);
			else
				target = (TextView)findViewById(R.id.fullhouse2);
			target.setText(Integer.toString(get_fullhouse()));
			target.setTextColor(Color.parseColor("#939995"));
		}

		if(p[curr].saved[9]==0){
			if(curr==0)
				target = (TextView)findViewById(R.id.smallstraight1);
			else
				target = (TextView)findViewById(R.id.smallstraight2);
			target.setText(Integer.toString(get_smallstraight()));
			target.setTextColor(Color.parseColor("#939995"));
		}

		if(p[curr].saved[10]==0){
			if(curr==0)
				target = (TextView)findViewById(R.id.largestraight1);
			else
				target = (TextView)findViewById(R.id.largestraight2);
			target.setText(Integer.toString(get_largestraight()));
			target.setTextColor(Color.parseColor("#939995"));
		}

		if(p[curr].saved[11]==0){
			if(curr==0)
				target = (TextView)findViewById(R.id.yacht1);
			else
				target = (TextView)findViewById(R.id.yacht2);
			target.setText(Integer.toString(get_yacht()));
			target.setTextColor(Color.parseColor("#939995"));
		}

	}
	protected void clear(){
		target = (TextView)findViewById(R.id.Aces1);
		target.setText("");
		target = (TextView)findViewById(R.id.Aces2);
		target.setText("");
		target = (TextView)findViewById(R.id.Duces1);
		target.setText("");
		target = (TextView)findViewById(R.id.Duces2);
		target.setText("");
		target = (TextView)findViewById(R.id.Threes1);
		target.setText("");
		target = (TextView)findViewById(R.id.Threes2);
		target.setText("");
		target = (TextView)findViewById(R.id.Fours1);
		target.setText("");
		target = (TextView)findViewById(R.id.Fours2);
		target.setText("");
		target = (TextView)findViewById(R.id.Fives1);
		target.setText("");
		target = (TextView)findViewById(R.id.Fives2);
		target.setText("");
		target = (TextView)findViewById(R.id.Sixes1);
		target.setText("");
		target = (TextView)findViewById(R.id.Sixes2);
		target.setText("");
		target = (TextView)findViewById(R.id.Subtotal1);
		target.setText("");
		target = (TextView)findViewById(R.id.Subtotal2);
		target.setText("");
		target = (TextView)findViewById(R.id.Bonus1);
		target.setText("");
		target = (TextView)findViewById(R.id.Bonus2);
		target.setText("");		
		target = (TextView)findViewById(R.id.Choice1);
		target.setText("");		
		target = (TextView)findViewById(R.id.Choice2);
		target.setText("");
		target = (TextView)findViewById(R.id.fourofakind1);
		target.setText("");		
		target = (TextView)findViewById(R.id.fourofakind2);
		target.setText("");
		target = (TextView)findViewById(R.id.fullhouse1);
		target.setText("");		
		target = (TextView)findViewById(R.id.fullhouse2);
		target.setText("");
		target = (TextView)findViewById(R.id.smallstraight1);
		target.setText("");	
		target = (TextView)findViewById(R.id.smallstraight2);
		target.setText("");		
		target = (TextView)findViewById(R.id.largestraight1);
		target.setText("");
		target = (TextView)findViewById(R.id.largestraight2);
		target.setText("");
		target = (TextView)findViewById(R.id.yacht1);
		target.setText("");		
		target = (TextView)findViewById(R.id.yacht2);
		target.setText("");		
		target = (TextView)findViewById(R.id.total1);
		target.setText("");	
		target = (TextView)findViewById(R.id.total2);
		target.setText("");
	}
	protected void Print_Raw(){
		clear();
		if(p[0].saved[0]==1){
			target = (TextView)findViewById(R.id.Aces1);
			target.setText(Integer.toString(p[0].Aces));
			target.setTextColor(Color.parseColor("#000000"));
		}
		if(p[1].saved[0]==1){
			target = (TextView)findViewById(R.id.Aces2);
			target.setText(Integer.toString(p[1].Aces));	
			target.setTextColor(Color.parseColor("#000000"));
		}

		if(p[0].saved[1]==1){
			target = (TextView)findViewById(R.id.Duces1);
			target.setText(Integer.toString(p[0].Duces));		
			target.setTextColor(Color.parseColor("#000000"));
		}
		if(p[1].saved[1]==1){
			target = (TextView)findViewById(R.id.Duces2);
			target.setText(Integer.toString(p[1].Duces));	
			target.setTextColor(Color.parseColor("#000000"));	
		}
		if(p[0].saved[2]==1){
			target = (TextView)findViewById(R.id.Threes1);
			target.setText(Integer.toString(p[0].Threes));		
			target.setTextColor(Color.parseColor("#000000"));
		}
		if(p[1].saved[2]==1){
			target = (TextView)findViewById(R.id.Threes2);
			target.setText(Integer.toString(p[1].Threes));		
			target.setTextColor(Color.parseColor("#000000"));
		}
		if(p[0].saved[3]==1){
			target = (TextView)findViewById(R.id.Fours1);
			target.setText(Integer.toString(p[0].Fours));		
			target.setTextColor(Color.parseColor("#000000"));
		}
		if(p[1].saved[3]==1){
			target = (TextView)findViewById(R.id.Fours2);
			target.setText(Integer.toString(p[1].Fours));		
			target.setTextColor(Color.parseColor("#000000"));
		}
		if(p[0].saved[4]==1){
			target = (TextView)findViewById(R.id.Fives1);
			target.setText(Integer.toString(p[0].Fives));	
			target.setTextColor(Color.parseColor("#000000"));
		}
		if(p[1].saved[4]==1){
			target = (TextView)findViewById(R.id.Fives2);
			target.setText(Integer.toString(p[1].Fives));		
			target.setTextColor(Color.parseColor("#000000"));
		}
		if(p[0].saved[5]==1){
			target = (TextView)findViewById(R.id.Sixes1);
			target.setText(Integer.toString(p[0].Sixes));		
			target.setTextColor(Color.parseColor("#000000"));
		}
		if(p[1].saved[5]==1){
			target = (TextView)findViewById(R.id.Sixes2);
			target.setText(Integer.toString(p[1].Sixes));		
			target.setTextColor(Color.parseColor("#000000"));
		}

		target = (TextView)findViewById(R.id.Subtotal1);
		target.setText(Integer.toString(p[0].subtotal)+"/63");		
		target.setTextColor(Color.parseColor("#000000"));

		target = (TextView)findViewById(R.id.Subtotal2);
		target.setText(Integer.toString(p[1].subtotal)+"/63");		
		target.setTextColor(Color.parseColor("#000000"));

		target = (TextView)findViewById(R.id.Bonus1);
		if(p[0].subtotal >= 64)
			target.setText("+35");	
		else
			target.setText("0");	
		target.setTextColor(Color.parseColor("#000000"));
		target = (TextView)findViewById(R.id.Bonus2);
		if(p[1].subtotal >= 64)
			target.setText("+35");	
		else
			target.setText("0");	
		target.setTextColor(Color.parseColor("#000000"));

		if(p[0].saved[6]==1){
			target = (TextView)findViewById(R.id.Choice1);
			target.setText(Integer.toString(p[0].Choice));		
			target.setTextColor(Color.parseColor("#000000"));
		}
		if(p[1].saved[6]==1){
			target = (TextView)findViewById(R.id.Choice2);
			target.setText(Integer.toString(p[1].Choice));	
			target.setTextColor(Color.parseColor("#000000"));
		}
		if(p[0].saved[7]==1){
			target = (TextView)findViewById(R.id.fourofakind1);
			target.setText(Integer.toString(p[0].fourofakind));		
			target.setTextColor(Color.parseColor("#000000"));
		}
		if(p[1].saved[7]==1){
			target = (TextView)findViewById(R.id.fourofakind2);
			target.setText(Integer.toString(p[1].fourofakind));	
			target.setTextColor(Color.parseColor("#000000"));
		}
		if(p[0].saved[8]==1){
			target = (TextView)findViewById(R.id.fullhouse1);
			target.setText(Integer.toString(p[0].fullhouse));	
			target.setTextColor(Color.parseColor("#000000"));
		}
		if(p[1].saved[8]==1){
			target = (TextView)findViewById(R.id.fullhouse2);
			target.setText(Integer.toString(p[1].fullhouse));	
			target.setTextColor(Color.parseColor("#000000"));
		}
		if(p[0].saved[9]==1){
			target = (TextView)findViewById(R.id.smallstraight1);
			target.setText(Integer.toString(p[0].smallstraight));		
			target.setTextColor(Color.parseColor("#000000"));
		}
		if(p[1].saved[9]==1){
			target = (TextView)findViewById(R.id.smallstraight2);
			target.setText(Integer.toString(p[1].smallstraight));	
			target.setTextColor(Color.parseColor("#000000"));
		}
		if(p[0].saved[10]==1){
			target = (TextView)findViewById(R.id.largestraight1);
			target.setText(Integer.toString(p[0].largestraight));		
			target.setTextColor(Color.parseColor("#000000"));
		}
		if(p[1].saved[10]==1){
			target = (TextView)findViewById(R.id.largestraight2);
			target.setText(Integer.toString(p[1].largestraight));	
			target.setTextColor(Color.parseColor("#000000"));
		}
		if(p[0].saved[11]==1){
			target = (TextView)findViewById(R.id.yacht1);
			target.setText(Integer.toString(p[0].yacht));		
			target.setTextColor(Color.parseColor("#000000"));
		}
		if(p[1].saved[11]==1){
			target = (TextView)findViewById(R.id.yacht2);
			target.setText(Integer.toString(p[1].yacht));	
			target.setTextColor(Color.parseColor("#000000"));
		}

		target = (TextView)findViewById(R.id.total1);
		if(p[0].subtotal >= 63)
			target.setText(Integer.toString(p[0].total+35));		
		else
			target.setText(Integer.toString(p[0].total));		
		target.setTextColor(Color.parseColor("#000000"));

		target = (TextView)findViewById(R.id.total2);
		if(p[1].subtotal >= 63)
			target.setText(Integer.toString(p[1].total+35));
		else
			target.setText(Integer.toString(p[1].total));	
		target.setTextColor(Color.parseColor("#000000"));
	}

	private int get_plane(int n){
		int result=0;
		for(int i=0;i<5;i++){
			if(dice[i]==n)
				result++;
		}
		return result*n;
	}
	private int get_choice(){
		int result =0;
		for(int i=0;i<5;i++)
			result += dice[i];
		return result;
	}
	private int get_fourofakind(){
		int flag=0;
		for(int i=1;i<=6;i++){
			flag =0;
			for(int j=0;j<5;j++){
				if(dice[j]==i)
					flag++;
			}
			if(flag>=4){
				return get_choice();
			}
		}
		return 0;
	}
	private int get_fullhouse(){

		int flag1=0;
		int flag2=0;
		for(int i=1;i<=6;i++){
			for(int j=1;j<=6;j++){
				flag1=0;
				flag2=0;
				if (j==i)
					continue;
				for(int k=0;k<5;k++){
					if(dice[k]==i)
						flag1++;
					if(dice[k]==j)
						flag2++;
				}
				if(flag1==2 && flag2==3)
					return 2*i+3*j;
				else if(flag1==3 && flag2==2)
					return 3*i+2*j;
				else if(flag1==5)
					return 5*i;
				else if(flag2==5)
					return 5*j;
			}
		}
		return 0;
	}

	private int get_smallstraight(){
		int flag[];
		flag = new int[7];
		for(int i=0;i<7;i++)
			flag[i]=0;
		for(int i=0;i<5;i++)
			flag[dice[i]]=1;


		if(flag[1]==1 && flag[2]==1 && flag[3] ==1 && flag[4] ==1)
			return 15;
		else if(flag[2]==1 && flag[3]==1 && flag[4] ==1 && flag[5] ==1)
			return 15;
		else if(flag[3]==1 && flag[4]==1 && flag[5] ==1 && flag[6] ==1)
			return 15;
		else 
			return 0;
	}

	private int get_largestraight(){
		int flag[];
		flag = new int[7];
		for(int i=0;i<7;i++)
			flag[i]=0;
		for(int i=0;i<5;i++)
			flag[dice[i]]=1;

		if(flag[1]==1 && flag[2]==1 && flag[3] ==1 && flag[4] ==1 && flag[5] ==1 )
			return 30;
		else if(flag[2]==1 && flag[3]==1 && flag[4] ==1 && flag[5] ==1 && flag[6] ==1 )
			return 30;
		else
			return 0;
	}

	private int get_yacht(){
		int flag =0;
		for(int i=1;i<=6;i++){
			flag=0;
			for(int j=0;j<5;j++){
				if(dice[j]==i)
					flag++;
			}
			if(flag==5){
				return 50;
			}
		}
		return 0;
	}


}


