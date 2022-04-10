flite
===============


Overview
---------------

Flite: a small run-time speech synthesis engine


Install
-----------------

.. code-block::

    git clone http://github.com/festvox/flite
    cd flite
    ./configure
    make
    make get_voices


Usage
------------------




.. code-block:: bash

    FLITEDIR := ~/workspace/exam/flite-2.0.0-release
    FLITEDIR_LIB := $(FLITEDIR)/build/x86_64-darwin14.5.0/lib

    build_flite_test:
            gcc -Wall -g -o ./flite_test flite_test.c -I$(FLITEDIR)/include -L$(FLITEDIR_LIB) -lflite_cmu_us_kal -lflite_usenglish -lflite_cmulex -lflite -lm
    run_flite_test:
            ./flite_test
    clean:
            rm -f flite_test
            rm -rf flite_test.dSYM

.. code-block:: c

    #include "flite.h"

    register_cmu_us_kal();

    int main(int argc, char **argv)
    {
        cst_voice *v;

        if (argc != 2)
        {
            fprintf(stderr,"usage: flite_test FILE\n");
            exit(-1);
        }

        flite_init();

        v = register_cmu_us_kal(NULL);

        flite_file_to_speech(argv[1],v,"play");

    }



  * text2wave


  .. code-block:: c

    #include <stdio.h>
    #include <string.h>
    #include "flite/flite.h"

    void usenglish_init(cst_voice *v);
    cst_lexicon *cmulex_init(void);

    //usage: text2wave("hello", "hello.wav", "./voices")
    int text2wave(const char* text, const char* outfile, const char *voice_pathname) {
        
        cst_voice *v;
        cst_wave *w;
        cst_utterance *u;
        const char *voice_pathname;

        /* Initialize Flite, and set up language and lexicon */
        flite_init();
        flite_add_lang("eng",usenglish_init,cmulex_init);
        flite_add_lang("usenglish",usenglish_init,cmulex_init);

        /* Load and select voice */
        v = flite_voice_select(voice_pathname);
        if (v == NULL)
        {
            fprintf(stderr,"failed to load voice: %s\n",voice_pathname);
            return 1;
        }

        u = flite_synth_text(text,v);
        w = utt_wave(u);

        /* Play the resulting wave, save it to a filename, or do none of these */
        cst_wave_save_riff(w,outfile);

        delete_utterance(u); /* will delete w too */

        return 0;
    }  