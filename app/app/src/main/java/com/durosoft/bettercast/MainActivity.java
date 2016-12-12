package com.durosoft.bettercast;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

import com.google.android.gms.appindexing.Action;
import com.google.android.gms.appindexing.AppIndex;
import com.google.android.gms.appindexing.Thing;
import com.google.android.gms.common.api.GoogleApiClient;

import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.nio.ByteBuffer;

public class MainActivity extends Activity implements SurfaceHolder.Callback {
    public static SurfaceView the_view = null;
    public static MainActivity the_activity = null;
    public static SurfaceHolder the_holder = null;
    /**
     * ATTENTION: This was auto-generated to implement the App Indexing API.
     * See https://g.co/AppIndexing/AndroidStudio for more information.
     */
    private GoogleApiClient client;

    /**
     * Called when the activity is first created.
     */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        clear_buffer();
        SurfaceView view = new SurfaceView(this);
        setContentView(view);
        view.getHolder().addCallback(this);
        the_view = view;
        the_activity = this;
        SeekDataTask task = new SeekDataTask();
        task.execute();
        // ATTENTION: This was auto-generated to implement the App Indexing API.
        // See https://g.co/AppIndexing/AndroidStudio for more information.
        client = new GoogleApiClient.Builder(this).addApi(AppIndex.API).build();
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        tryDrawing(holder);
        the_holder = holder;
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int frmt, int w, int h) {
        tryDrawing(holder);
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
    }

    public void tryDrawing(SurfaceHolder holder) {
        Canvas canvas = holder.lockCanvas();
        if (canvas != null) {
            drawMyStuff(canvas);
            holder.unlockCanvasAndPost(canvas);
        } else {
            System.out.println("CANT DRAW");
        }
    }

    final int MAX_SCREEN_SIZE = 1920 * 1080;
    final int PATCH_SIZE = 12;
    int screen_buffer[] = new int[MAX_SCREEN_SIZE];
    byte byte_buffer[] = new byte[MAX_SCREEN_SIZE * 3];
    int screen_w = 0;
    int screen_h = 0;
    int patch_pos_x = -1;
    int patch_pos_y = -1;
    String sp[];

    Paint paint = new Paint();
    //Bitmap bmp = Bitmap.createBitmap(1080, 1920, Bitmap.Config.ARGB_8888);

    private void clear_buffer() {
        System.out.println("clearing buffer");
        for (int i = 0; i < screen_buffer.length; i++) {
            screen_buffer[i] = i;
        }
    }

    private void short_pause() {
        try {
            Thread.currentThread().sleep(2000);
        } catch (InterruptedException ee) {
        }
    }

    public static void cleanup_connection() {
        if (out != null)
            out.close();
        if (in != null)
            try {
                in.close();
            } catch (IOException ee) {
            }
        try {
            if(socket != null)
            socket.close();
        } catch (IOException ee) {
        }
        socket = null;
        in = null;
        out = null;
    }

    public static Socket socket = null;
    public static PrintWriter out = null;
    public static DataInputStream in = null;

    /**
     * ATTENTION: This was auto-generated to implement the App Indexing API.
     * See https://g.co/AppIndexing/AndroidStudio for more information.
     */
    public Action getIndexApiAction() {
        Thing object = new Thing.Builder()
                .setName("Main Page") // TODO: Define a title for the content shown.
                // TODO: Make sure this auto-generated URL is correct.
                .setUrl(Uri.parse("http://[ENTER-YOUR-URL-HERE]"))
                .build();
        return new Action.Builder(Action.TYPE_VIEW)
                .setObject(object)
                .setActionStatus(Action.STATUS_TYPE_COMPLETED)
                .build();
    }

    @Override
    public void onStart() {
        super.onStart();

        // ATTENTION: This was auto-generated to implement the App Indexing API.
        // See https://g.co/AppIndexing/AndroidStudio for more information.
        client.connect();
        AppIndex.AppIndexApi.start(client, getIndexApiAction());
        the_view.setLayerType(View.LAYER_TYPE_HARDWARE, null);
    }

    @Override
    public void onStop() {
        super.onStop();

        // ATTENTION: This was auto-generated to implement the App Indexing API.
        // See https://g.co/AppIndexing/AndroidStudio for more information.
        AppIndex.AppIndexApi.end(client, getIndexApiAction());
        client.disconnect();
    }

    private class SeekDataTask extends AsyncTask<Boolean, Boolean, Boolean> {
        @Override
        protected Boolean doInBackground(Boolean... o) {
            if (socket == null) {
                System.out.println("trying to connect...");
                try {
                    socket = new Socket("sam-ThinkPad-T440s", 13314);
                } catch (IOException e) {
                    System.out.println(e.getMessage());
                    System.out.println("error connecting");
                    cleanup_connection();
                    short_pause();
                    return false;
                }
            } else {
                //System.out.println("already connected");
            }

            System.out.println("connected");
            try {
                if (out == null)
                    out = new PrintWriter(socket.getOutputStream(), true);
                if (in == null)
                    in = new DataInputStream(socket.getInputStream());
                if(screen_w == 0 || screen_h == 0) {
                    out.write("size\r\n");
                    out.flush();
                    String size_data = in.readLine();
                    sp = size_data.split("x");
                    screen_w = Integer.parseInt(sp[0]);
                    screen_h = Integer.parseInt(sp[1]);
                    System.out.println("size: " + screen_w + "x" + screen_h);
                }
                out.write("diff\r\n");
                out.flush();
                String resp = in.readLine();
                if(resp.equals("no change")) {
                    System.out.println("no change");
                } else if(resp.equals("full refresh")) {
                    System.out.println("full refresh");
                    in.readFully(byte_buffer, 0, byte_buffer.length);
                    for(int i = 0; i < byte_buffer.length; i+=3) {
                        screen_buffer[i / 3] = (255 << 32) +
                                (byte_buffer[i] << 16) +
                                (byte_buffer[i + 1] << 8) +
                                (byte_buffer[i + 2]);
                    }
                    in.readLine(); // should read "ok"
                } else if(resp.equals("diff incoming")) {
                    System.out.println("partial diff");
                    while(true) {
                        resp = in.readLine();
                        if(resp.equals("ok")) break;
                        sp = resp.split(","); // try to mitigate dynamic memory allocation
                        patch_pos_x = Integer.parseInt(sp[0]);
                        patch_pos_y = Integer.parseInt(sp[1]);
                        System.out.println("patch pos x: " + patch_pos_x);
                        System.out.println("patch pos y: " + patch_pos_y);
                        in.readFully(byte_buffer, 0, PATCH_SIZE * PATCH_SIZE * 3);
                    }
                }
            } catch (Exception e) {
                if(e != null) System.out.println(e.getMessage());
                System.out.println("connection error");
                cleanup_connection();
                return false;
            }
            return true;
        }

        protected void onPostExecute(Boolean arg) {
            the_activity.tryDrawing(the_holder);
            SeekDataTask task = new SeekDataTask();
            task.execute();
        }
    }

    private void drawMyStuff(final Canvas canvas) {
        //System.out.println("drawing");
        canvas.drawBitmap(screen_buffer, 0, screen_w, 0, 0, screen_w, screen_h, false, paint);
    }
}