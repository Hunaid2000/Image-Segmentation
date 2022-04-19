package application;

import java.io.File;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;
import java.util.ResourceBundle;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.fxml.Initializable;
import javafx.scene.Node;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;
import javafx.scene.layout.AnchorPane;
import javafx.stage.FileChooser;
import javafx.stage.Stage;
import javafx.stage.FileChooser.ExtensionFilter;

public class AppController implements Initializable {
	
	List<String> files = new ArrayList<String>();

    @FXML
    private TextField Clusters;

    @FXML
    private Button FileChooseButton;

    @FXML
    private Label FileName;

    @FXML
    private TextField MaxIter;

    @FXML
    private Button StartButton;

    @FXML
    void ChooseFileAction(ActionEvent event) {
    	FileChooser fileChooser = new FileChooser();
    	fileChooser.getExtensionFilters().add(new ExtensionFilter("Image Files", files));
    	File file = fileChooser.showOpenDialog(null);
    	if(file != null) {
    		FileName.setText(file.getAbsolutePath());
    	}    	
    }

    @FXML
    void StartAction(ActionEvent event) {
    	new Main().KmeansSegmentation(FileName.getText(), Integer.valueOf(Clusters.getText()), Integer.valueOf(MaxIter.getText()));
    }

	@Override
	public void initialize(URL arg0, ResourceBundle arg1) {
		files.add("*.jpg");
		
	}

}
