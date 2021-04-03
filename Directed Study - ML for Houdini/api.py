import flask
from flask import request, jsonify
import os
from run_model import run_models

app = flask.Flask(__name__)
app.config["DEBUG"] = True


@app.route('/flask-example', methods=['GET'])
def home():
    if 'test_image' in request.args:
        run_string=run_models(request.args['test_image'])
        # os.system(run_string)
        print(run_string)
        return(run_string)

app.run()