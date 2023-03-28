from flask import Flask, request
app = Flask(__name__)

# import owner_lib as ow

users = []

@app.route('/owners', methods=['POST'])
def create_user():
    data = request.get_json()

    #ret = ow.create_owner()

    # mandatory fields
    name = data.get('name')
    mobile = data.get('mobile')
    secret = data.get('secret')

    # optional fields
    dp = data.get('profile_photo')
    address = data.get('address')
    dob = data.get('dob')
    email = data.get('email')
	
    if not name or not mobile or not secret:
        return "Bad request: name, mobile, and secret are required fields", 400
	
    user = {"name": name, "mobile": mobile, "secret": secret, 'profile_photo': dp, 'address':address, 'dob': dob, 'email': email }

	# before try to add user into db do required validatoions i.e. check is user already exist
	# If user already exist
	# return "Bad request: User already exist", 400
	# store above user to db/file for time begin
	
    users.append(user)

    return "User created\n", 201


@app.route('/owners', methods=['GET'])
def get_user():
    user_cnt = 'Total users:' + str(len(users)) + '\n'

    return user_cnt,  200 

if __name__ == "__main__":
    app.run(debug=True)
