@echo OFF

Pushd %GRIMDAT_PATH%
python grimDat.py -i engine.grimDat -o Generated -j type_header.jinja -p Include.props
python grimDat.py -i engine.grimDat -o Generated -j type_source.jinja -p Source.props
popd
