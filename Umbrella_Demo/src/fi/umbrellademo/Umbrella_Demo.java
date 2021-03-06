/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package fi.umbrellademo;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.WindowManager;

import android.view.MotionEvent;
import android.widget.Toast;

import java.io.File;


public class Umbrella_Demo extends Activity {

    UmbrellaDemoView mView;

	private boolean isTouch = false;

    @Override protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);
        mView = new UmbrellaDemoView(getApplication());
	setContentView(mView);
    }

    @Override protected void onPause() {
        super.onPause();
        mView.onPause();
    }

    @Override protected void onResume() {
        super.onResume();
        mView.onResume();
    }
	@Override
public boolean onTouchEvent(MotionEvent event)
{

    int X = (int) event.getX();
    int Y = (int) event.getY();

    int eventaction = event.getAction();

    switch (eventaction) {

    case MotionEvent.ACTION_DOWN:

        Toast.makeText(this, "DON'T TOUCH THIS",             Toast.LENGTH_SHORT).show();		// TOUCH EVENT!!!

        isTouch = true;
        break;

    }

    return true;

}
}
