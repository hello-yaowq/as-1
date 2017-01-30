/**
 * AS - the open source Automotive Software on https://github.com/parai
 *
 * Copyright (C) 2017  AS <parai@foxmail.com>
 *
 * This source code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by the
 * Free Software Foundation; See <http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */
import java.util.Timer;
import java.util.TimerTask;

class TimerEvent extends TimerTask {
	private int counter = 0;

	public TimerEvent() {
		System.out.println("Constructor of TimerEvent!");
	}

	private void onTimerEvent() {
		counter ++;
		System.out.println(counter);
	}

	@Override
	public void run() {
		onTimerEvent();
	}
}

public class main {

	private static Timer timer;

	public static void main(String[] argv) {

		timer = new Timer();	//creates a new timer to schedule
		timer.schedule(new TimerEvent(), 400, 400);
		
		System.out.println("Hello World!");
	}
}

