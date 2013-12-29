cp source/conf.py source/autoscript
cp -r source/_themes source/autoscript
cd source/autoscript
make html
rm -r _themes
rm conf.py
cd ../../
