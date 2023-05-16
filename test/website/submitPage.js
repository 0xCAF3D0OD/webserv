const input = document.querySelector('input');
const preview = document.querySelector('.preview');
const itemsList = document.getElementById('items-hook');
const submitBtn = document.querySelector('#submit-btn');

let urlUpload = 'http://localhost:8081/uploads';
let url = '';

let submitedFiles = [];

// Handle the front when a file is selected, preview.
const updateImageDisplay = () => {
	const curFiles = input.files[0];

	// display the image preview
	const list = document.createElement('ul');
	list.style.listStyleType = 'none';
	preview.appendChild(list);
	const listItem = document.createElement('li');
	if (validFileType(curFiles))
	{
		const image = document.createElement('img');
		image.style.width = '250px';
		image.style.height = '150px';
		image.src = URL.createObjectURL(curFiles);
		if (curFiles.type === 'application/pdf')
			image.src = './PDF_file_icon.png'
			listItem.appendChild(image);
	}
	else
	{
		para.textContent = `File name ${curFiles.name}: Not a valid file type. Update your selection.`;
		listItem.appendChild(para);
	}
	list.appendChild(listItem);
};

const validFileType = (file) => { return fileTypes.includes(file.type); };

const fileTypes =
	[ 'image/gif', 'image/jpeg', 'image/png', 'image/svg+xml', 'image/x-icon', 'application/pdf' ];

const renderImages = () => {
	while (itemsList.firstChild)
	{
		itemsList.removeChild(itemsList.firstChild);
	}
	if (submitedFiles.length === 0)
		return;
	for (const images of submitedFiles)
	{
		itemsList.appendChild(images);
	}
	while (preview.firstChild)
	{
		preview.removeChild(preview.firstChild);
	}
	input.value = null;
};

// Function that is called when we click on a existing file to delete it
const deleteExistingFile = (e) => {
	e.preventDefault();
	const id = e.target.parentElement.id;
	// submitedFiles = submitedFiles.filter((el) => el.id !== e.target.parentElement.id);
	submitedFiles.forEach((el, idx) => {
		if (el.id === e.target.parentElement.id)
		{
			submitedFiles.splice(idx, 1);
			return;
		}
	})

	fetch(`${urlUpload}/${id}`, { method : 'DELETE' })
		.then((response) => {
			if (!response.ok)
			{
				throw new Error('Network response was not ok');
			}
			console.log('Item deleted successfully');
		})
		.catch((error) => { console.error('There was a problem deleting the item:', error); });
	renderImages();
};

const submitHandler = (e) => {
	e.preventDefault();
	// Backend
	const curFiles = input.files;
	const data = new FormData();
	const files = curFiles[0].name;
	data.append(files, curFiles[0]);
	fetch(urlUpload, { method : 'POST', mode : 'no-cors', body : data })
		.then((response) => response)
		.then((data) => { console.log(data); })
		.catch((error) => { console.error(error); });
	// Frontend
	if (curFiles.length === 0)
		return;
	const li = document.createElement('li');
	li.setAttribute('id', files);
	const image = document.createElement('img');
	image.style.width = '250px';
	image.style.height = '150px';
	image.src = URL.createObjectURL(curFiles[0]);
	if (curFiles[0].type === 'application/pdf')
		image.src = './PDF_file_icon.png'
		li.appendChild(image);
	li.addEventListener('click', deleteExistingFile);
	submitedFiles.push(li);
	renderImages();
};

input.addEventListener('change', updateImageDisplay);
submitBtn.addEventListener('click', submitHandler);
