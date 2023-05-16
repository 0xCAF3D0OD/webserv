const input = document.querySelector('input');
const preview = document.querySelector('.preview');
const itemsList = document.getElementById('items-hook');
const submitBtn = document.querySelector('#submit-btn');

let urlUpload = 'http://localhost:8081/cgi/input.php';
let url = '';

const submitHandler = (e) => {
	e.preventDefault();
	if (input.value.trim().length > 0)
	{
		fetch(urlUpload, { method : 'POST', mode : 'no-cors', body : input.value })
		.then((response) => response)
		.then((data) => { console.log(data); })
		.catch((error) => { console.error(error); });
	}
	input.value = "";
}



submitBtn.addEventListener('click', submitHandler);
