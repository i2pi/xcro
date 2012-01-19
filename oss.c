void init_oss (const char* name)
{
    int openmode = O_RDONLY;
	int	channels = 2;
    const int want_format = AFMT_S16_NE;
	int	buffer_size;
	int	s  = 8;
    int arg = 0x00020000 + s;
    int tmp_format = AFMT_S16_NE;
    int tmp_stereo = 0;
    int tmp_rate = 22050;


    fd = open (name, openmode, 0);
    if (fd == -1) {
		fprintf (stderr, "Fatal: failed to open soundcard\n");
		exit (-1);
    }

    if (ioctl (fd, SNDCTL_DSP_SETFRAGMENT, &arg)) {
		fprintf (stderr, "Fatal: cant set framgent\n");
		exit (-1);
    }

    if (ioctl (fd, SNDCTL_DSP_SETFMT, &tmp_format) == -1) {
		fprintf (stderr, "Fatal: couldnt set format\n");
		exit (-1);
    }


    if (ioctl (fd, SNDCTL_DSP_STEREO, &tmp_stereo) == -1) {
		fprintf (stderr, "Fatal: couldnt get mono\n");
		exit (-1);
    }


    if (ioctl (fd, SNDCTL_DSP_SPEED, &tmp_rate) == -1) {
		fprintf (stderr, "Fatal: couldnt get rate\n");
		exit (-1);
    }

	printf ("Got audio rate %ld\n", tmp_rate);

    if (ioctl (fd, SNDCTL_DSP_GETBLKSIZE, &buffer_size) == -1) {
		fprintf (stderr, "Fatal: couldnt get buffer size\n");
		exit (-1);
    }

	printf ("Got buffer size %ld\n", buffer_size);
}

int main (int argc, char **argv)
{
	short	buf[1024];
	int		i;

	init_oss ("/dev/dsp" );

	while (1)
	{
		read(fd, buf, 256*sizeof(short));
		for (i=0; i<256; i++)
		{
			printf ("%7d ", buf[i]);
		}
	}

	return (0);
}
