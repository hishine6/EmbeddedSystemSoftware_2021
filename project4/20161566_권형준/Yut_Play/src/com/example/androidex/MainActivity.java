package com.example.androidex;

import android.app.Activity;
import android.content.Intent;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;


public class MainActivity extends Activity{
	MediaPlayer mp1;
	int mp_cnt=0;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		
		mp1=MediaPlayer.create(this,R.raw.soundeffect);
		Button btn = (Button)findViewById(R.id.music);
		mp1.start();
		mp1.setLooping(true);
		OnClickListener ltn=new OnClickListener(){
			public void onClick(View v){
				mp_cnt++;
				if(mp_cnt%2==0){
					mp1.start();
					mp1.setLooping(true);
				}
				else
					mp1.pause();
			}
		};
		btn.setOnClickListener(ltn);


		Button btn1=(Button)findViewById(R.id.newactivity);
		OnClickListener listener=new OnClickListener(){
			public void onClick(View v){
				Intent intent=new Intent(MainActivity.this, MultiPlayer.class);
				startActivity(intent);
			}
		};
		btn1.setOnClickListener(listener);
		
		Button btn2=(Button)findViewById(R.id.End);
		OnClickListener listener2=new OnClickListener(){
			public void onClick(View v){
				mp1.release();
				finish();
			}
		};
		btn2.setOnClickListener(listener2);

	}

}
