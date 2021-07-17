package com.example.androidex;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;



public class Result_View extends Activity{
		@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main3);
		String winner = getIntent().getStringExtra("winner");
		int player1[] = getIntent().getIntArrayExtra("player1");
		int player2[] = getIntent().getIntArrayExtra("player2");
		
		if(winner.equals("0")){
			TextView temp = (TextView)findViewById(R.id.result);
			temp.setText("Player 1 Wins!");
		}
		else{
			TextView temp = (TextView)findViewById(R.id.result);
			temp.setText("Player 2 Wins!"); 
		}
		TextView temp;
		
		temp = (TextView)findViewById(R.id.result_Aces1);
		temp.setText(Integer.toString(player1[0]));
		temp = (TextView)findViewById(R.id.result_Aces2);
		temp.setText(Integer.toString(player2[0]));
		
		temp = (TextView)findViewById(R.id.result_Duces1);
		temp.setText(Integer.toString(player1[1]));
		temp = (TextView)findViewById(R.id.result_Duces2);
		temp.setText(Integer.toString(player2[1]));
		
		temp = (TextView)findViewById(R.id.result_Threes1);
		temp.setText(Integer.toString(player1[2]));
		temp = (TextView)findViewById(R.id.result_Threes2);
		temp.setText(Integer.toString(player2[2]));
		
		temp = (TextView)findViewById(R.id.result_Fours1);
		temp.setText(Integer.toString(player1[3]));
		temp = (TextView)findViewById(R.id.result_Fours2);
		temp.setText(Integer.toString(player2[3]));
		
		temp = (TextView)findViewById(R.id.result_Fives1);
		temp.setText(Integer.toString(player1[4]));
		temp = (TextView)findViewById(R.id.result_Fives2);
		temp.setText(Integer.toString(player2[4]));
		
		temp = (TextView)findViewById(R.id.result_Sixes1);
		temp.setText(Integer.toString(player1[5]));
		temp = (TextView)findViewById(R.id.result_Sixes2);
		temp.setText(Integer.toString(player2[5]));
		
		temp = (TextView)findViewById(R.id.result_Choice1);
		temp.setText(Integer.toString(player1[6]));
		temp = (TextView)findViewById(R.id.result_Choice2);
		temp.setText(Integer.toString(player2[6]));
		
		temp = (TextView)findViewById(R.id.result_fourofakind1);
		temp.setText(Integer.toString(player1[7]));
		temp = (TextView)findViewById(R.id.result_fourofakind2);
		temp.setText(Integer.toString(player2[7]));
		
		temp = (TextView)findViewById(R.id.result_fullhouse1);
		temp.setText(Integer.toString(player1[8]));
		temp = (TextView)findViewById(R.id.result_fullhouse2);
		temp.setText(Integer.toString(player2[8]));
		
		temp = (TextView)findViewById(R.id.result_smallstraight1);
		temp.setText(Integer.toString(player1[9]));
		temp = (TextView)findViewById(R.id.result_smallstraight2);
		temp.setText(Integer.toString(player2[9]));
		
		temp = (TextView)findViewById(R.id.result_largestraight1);
		temp.setText(Integer.toString(player1[10]));
		temp = (TextView)findViewById(R.id.result_largestraight2);
		temp.setText(Integer.toString(player2[10]));
		
		temp = (TextView)findViewById(R.id.result_yacht1);
		temp.setText(Integer.toString(player1[11]));
		temp = (TextView)findViewById(R.id.result_yacht2);
		temp.setText(Integer.toString(player2[11]));
		
		temp = (TextView)findViewById(R.id.result_total1);
		temp.setText(Integer.toString(player1[12]));
		temp = (TextView)findViewById(R.id.result_total2);
		temp.setText(Integer.toString(player2[12]));
		
		temp = (TextView)findViewById(R.id.result_Subresult_total1);
		temp.setText(Integer.toString(player1[13])+"/63");
		temp = (TextView)findViewById(R.id.result_Subresult_total2);
		temp.setText(Integer.toString(player2[13])+"/63");
		
		temp = (TextView)findViewById(R.id.result_Bonus1);
		temp.setText("+"+Integer.toString(player1[14]));
		temp = (TextView)findViewById(R.id.result_Bonus2);
		temp.setText("+"+Integer.toString(player2[14]));
		
		Button btn1=(Button)findViewById(R.id.Back);
		OnClickListener listener=new OnClickListener(){
			public void onClick(View v){
				finish();
			}
		};
		btn1.setOnClickListener(listener);
	}

}
